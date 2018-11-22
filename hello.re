open Lwt;
open Cohttp_lwt_unix;

let ok = x => (`OK, x);

let lwt_of_option = (exn, opt) =>
  switch (opt) {
  | Some(a) => Lwt.return(a)
  | None => Lwt.fail(exn)
  };

let generate = () => {
  open Models;
  let values = {state: Pending, description: "Waiting", context: "pr/labels"};
  json_of_status_post_payload(values);
};

exception ServerError(Cohttp.Code.status_code, string);

let startsWith = (substr, str) =>
  Stringext.find_from(~pattern=substr, str) != None;
let isValidLabel = startsWith("PR: ");

let handlePullRequest = (token, body) => {
  open Models;
  let pr = Ezjsonm.from_string(body) |> pull_request_of_json;
  let statusPayload =
    switch (List.find(isValidLabel)) {
    | _ => Models.successPayload
    | exception Not_found => Models.pendingPayload
    };
  let%lwt response =
    Http.post(
      ~body=Models.json_of_status_post_payload(statusPayload),
      Printf.sprintf("%s?access_token=%s", pr.statuses_url, token),
    );

  ok(response) |> Lwt.return;
};

let unwrapServerErrors = res =>
  try%lwt (res) {
  | ServerError(status, message) => Lwt.return((status, message ++ "\n"))
  };

let server = port => {
  let callback = (_conn, req, requestBody) => {
    /* let uri = req |> Request.uri |> Uri.to_string; */
    let meth = req |> Request.meth;
    /* let headers = req |> Request.headers |> Header.to_string; */
    let uri = req |> Request.uri;
    let path = uri |> Uri.path;
    let q = (errorMessage, name) =>
      Uri.get_query_param(uri, name)
      |> lwt_of_option(ServerError(`Bad_request, errorMessage));

    let newBody =
      switch (meth, path) {
      | (`GET, "/") =>
        Http.post(~body=generate(), "http://example.com") >|= ok
      | (`POST, "/handle_pull_request") =>
        let%lwt token = q("Token is mandatory", "token")
        and body = Cohttp_lwt.Body.to_string(requestBody);
        let x = handlePullRequest(token, body);
        x;
      | _ => Lwt.return((`Not_found, "Unknown route."))
      };

    newBody
    |> unwrapServerErrors
    >>= (((status, body)) => Server.respond_string(~status, ~body, ()));
  };
  Server.create(~mode=`TCP(`Port(port)), Server.make(~callback, ()));
};

let port =
  switch (Sys.argv[1]) {
  | str => int_of_string(str)
  | exception Invalid_argument(_) => 3000
  };
Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

server(port) |> Lwt_main.run |> ignore;
