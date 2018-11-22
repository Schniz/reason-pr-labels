open Lwt;
open Cohttp_lwt_unix;

let ok = x => (`OK, x);

let lwt_of_option = (exn, opt) =>
  switch (opt) {
  | Some(a) => Lwt.return(a)
  | None => Lwt.fail(exn)
  };

exception ServerError(Cohttp.Code.status_code, string);

let startsWith = (substr, str) =>
  Stringext.find_from(~pattern=substr, str) != None;
let isValidLabel = startsWith("PR: ");

let handlePullRequest = (token, body) => {
  let%lwt response =
    Ezjsonm.from_string(body)
    |> PullRequest.of_json
    |> PullRequest.sendStatus(token);
  ok(response) |> Lwt.return;
};

let unwrapServerErrors = res =>
  try%lwt (res) {
  | ServerError(status, message) => Lwt.return((status, message ++ "\n"))
  };

let server = port => {
  let callback = (_conn, req, requestBody) => {
    let meth = req |> Request.meth;
    let uri = req |> Request.uri;
    let path = uri |> Uri.path;
    let q = (errorMessage, name) =>
      Uri.get_query_param(uri, name)
      |> lwt_of_option(ServerError(`Bad_request, errorMessage));

    let response =
      switch (meth, path) {
      | (`GET, "/") => ok("Server is up") |> Lwt.return
      | (`POST, "/handle_pull_request") =>
        let%lwt token = q("Token is mandatory", "token")
        and body = Cohttp_lwt.Body.to_string(requestBody);
        handlePullRequest(token, body);
      | _ => Lwt.return((`Not_found, "Unknown route."))
      };

    response
    |> unwrapServerErrors
    >>= (((status, body)) => Server.respond_string(~status, ~body, ()));
  };
  Server.create(~mode=`TCP(`Port(port)), Server.make(~callback, ()));
};

let port =
  switch (Sys.argv[1]) {
  | str => int_of_string(str)
  | exception (Invalid_argument(_)) => 3000
  };
Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

server(port) |> Lwt_main.run |> ignore;
