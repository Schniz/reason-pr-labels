let string_of_file_path = path => {
  let ic = open_in(path);
  let len = in_channel_length(ic);
  let s = really_input_string(ic, len);
  close_in(ic);
  s;
};

let port =
  switch (Sys.argv[1]) {
  | str => int_of_string(str)
  | exception (Invalid_argument(_)) => 3000
  };

let `RSA(key) =
  string_of_file_path("./github.private-key.pem")
  |> Cstruct.of_string
  |> X509.Encoding.Pem.Private_key.of_pem_cstruct1;

let webhookSecret = {
  let value = Sys.getenv_opt("GITHUB_WEBHOOK_SECRET");
  if (value == None) {
    "Warning: You're running the app without a webhook secret.\n"
    ++ "If you want to use a secret, set the `GITHUB_WEBHOOK_SECRET` environment variable.\n"
    |> print_endline;
  };
  value;
};

Printf.sprintf("Listening on %d. Ctrl-C to quit!", port) |> print_endline;

Server.make(~port, ~key, ~webhookSecret) |> Lwt_main.run |> ignore;
