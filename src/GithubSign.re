let rs256_sign = (key, data) => {
  /* Taken from https://github.com/mmaker/ocaml-letsencrypt */
  let data = Cstruct.of_string(data);
  let h = Nocrypto.Hash.SHA256.digest(data);
  let pkcs1_digest = X509.Encoding.pkcs1_digest_info_to_cstruct((`SHA256, h));
  Nocrypto.Rsa.PKCS1.sig_encode(~key, pkcs1_digest) |> Cstruct.to_string;
};

let base64 = B64.encode(~pad=false, ~alphabet=B64.uri_safe_alphabet);

let makeJwt = key => {
  let header = "{ \"alg\": \"RS256\" }";
  let issuedAt = Unix.time() |> int_of_float;
  let payload =
    Printf.sprintf(
      "{ \"iat\": %d, \"exp\": %d, \"iss\": 21259 }",
      issuedAt,
      issuedAt + 60 * 10,
    );
  let m = base64(header) ++ "." ++ base64(payload);
  let signature = rs256_sign(key, m) |> base64;
  Http.Bearer(m ++ "." ++ signature);
};
