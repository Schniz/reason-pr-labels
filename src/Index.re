let string_of_file_path = path => {
  let ic = open_in(path);
  let len = in_channel_length(ic);
  let s = really_input_string(ic, len);
  close_in(ic);
  s;
};

let `RSA(key) =
  string_of_file_path("./github.private-key.pem")
  |> Cstruct.of_string
  |> X509.Encoding.Pem.Private_key.of_pem_cstruct1;

let config = Config.get();
let port = Config.port(config);

let webhookSecret = {
  let value = Config.webhookSecret(config);
  if (value == None) {
    "Warning: You're running the app without a webhook secret.\n"
    ++ "If you want to use a secret, set the `GITHUB_WEBHOOK_SECRET` environment variable.\n"
    |> print_endline;
  };
  value;
};


Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

Server.make(~port, ~key, ~webhookSecret) |> Lwt_main.run |> ignore;
