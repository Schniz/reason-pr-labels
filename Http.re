open Lwt;

let jsonHeaders = Cohttp.Header.init_with("Content-Type", "application/json");

type response =
  | Ok(string)
  | Error(string);

let post = (~body as jsonBody, url) => {
  let body = Ezjsonm.to_string(jsonBody) |> Cohttp_lwt.Body.of_string;
  Cohttp_lwt_unix.Client.post(
    ~body,
    ~headers=jsonHeaders,
    Uri.of_string(url),
  )
  >>= (
    ((_response, body)) =>
      body |> Cohttp_lwt.Body.to_string >|= (body => "Hello! " ++ body)
  );
};
