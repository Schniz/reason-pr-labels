open Lwt;

let jsonHeaders = Cohttp.Header.init_with("Content-Type", "application/json");

type response =
  | Ok(string)
  | Error(string);

let post = (~body, url) =>
  Cohttp_lwt_unix.Client.post(
    ~body=Cohttp_lwt.Body.of_string(Ezjsonm.to_string(body)),
    ~headers=jsonHeaders,
    Uri.of_string(url),
  )
  >>= (
    ((_response, body)) =>
      body
      |> Cohttp_lwt.Body.to_string
      >|= (body => "Hello! " ++ body)
  );
