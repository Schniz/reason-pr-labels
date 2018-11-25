open Lwt;
open Cohttp_lwt_unix;

exception ServerError(Cohttp.Code.status_code, string);
let ok = x => (`OK, x);

let lwt_of_option = (exn, opt) =>
  switch (opt) {
  | Some(a) => Lwt.return(a)
  | None => Lwt.fail(exn)
  };

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

let calculateHash = (~key, body) =>
  Printf.sprintf(
    "sha1=%s",
    Cstruct.of_string(body)
    |> Nocrypto.Hash.SHA1.hmac(~key=Cstruct.of_string(key))
    |> Hex.of_cstruct
    |> Hex.show
  );

let verifyBody = (~secret, ~headers, body) =>
  switch (secret, Cohttp.Header.get(headers, "x-hub-signature")) {
  | (None, None)
  | (None, Some(_)) => Lwt.return(body)
  | (Some(_), None) =>
    Lwt.fail(
      ServerError(
        `Bad_request,
        "Webhook must calculate signature with secret.",
      ),
    )
  | (Some(secret), Some(signature)) =>
    let calculatedHash = calculateHash(~key=secret, body);
    Printf.sprintf("given: %s, calculated: %s", signature, calculatedHash)
    |> print_endline;
    calculateHash(~key=secret, body) == signature ?
      Lwt.return(body) :
      Lwt.fail(ServerError(`Unauthorized, "Webhook secret didn't match."));
  };

let make = (~port, ~key, ~webhookSecret) => {
  let callback = (_conn, req, requestBody) => {
    let meth = req |> Request.meth;
    let uri = req |> Request.uri;
    let path = uri |> Uri.path;
    let headers = req |> Request.headers;

    let response =
      switch (meth, path) {
      | (`GET, "/")
      | (`GET, "/welcome") => ok("Welcome to the service!") |> Lwt.return
      | (`POST, "/handle_pull_request") =>
        let%lwt body =
          Cohttp_lwt.Body.to_string(requestBody)
          >>= verifyBody(~secret=webhookSecret, ~headers);
        handlePullRequest(key, body);
      | _ => Lwt.return((`Not_found, "Unknown route."))
      };

    response
    |> unwrapServerErrors
    >>= (((status, body)) => Server.respond_string(~status, ~body, ()));
  };
  Server.create(~mode=`TCP(`Port(port)), Server.make(~callback, ()));
};
