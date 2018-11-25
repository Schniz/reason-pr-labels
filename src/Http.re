open Lwt;

type response =
  | Ok(string)
  | Error(string);

type auth =
  | Bearer(string)
  | Token(string);

let string_of_auth =
  fun
  | Bearer(token) => "Bearer " ++ token
  | Token(token) => "token " ++ token;

let addHeader = (name, value, headers) =>
  Cohttp.Header.add(headers, name, value);

let jsonHeaders = (~auth) =>
  Cohttp.Header.init_with("Content-Type", "application/json")
  |> addHeader("accept", "application/vnd.github.machine-man-preview+json")
  |> addHeader("authorization", string_of_auth(auth));

let get = (~auth, url) => {
  print_endline("Making get request to " ++ url);
  Cohttp_lwt_unix.Client.get(
    ~headers=jsonHeaders(~auth),
    Uri.of_string(url),
  )
  >>= (((_response, body)) => body |> Cohttp_lwt.Body.to_string);
};

let post = (~body as jsonBody=?, ~auth, url) => {
  print_endline("Making post request to " ++ url);
  let body =
    (
      switch (jsonBody) {
      | None => "{}"
      | Some(json) => Ezjsonm.to_string(json)
      }
    )
    |> Cohttp_lwt.Body.of_string;
  Cohttp_lwt_unix.Client.post(
    ~body,
    ~headers=jsonHeaders(~auth),
    Uri.of_string(url),
  )
  >>= (((_response, body)) => body |> Cohttp_lwt.Body.to_string);
};
