let rs256_sign = (key, data) => {
  let data = Cstruct.of_string(data);
  let h = Nocrypto.Hash.SHA256.digest(data);
  let pkcs1_digest = X509.Encoding.pkcs1_digest_info_to_cstruct((`SHA256, h));
  Nocrypto.Rsa.PKCS1.sig_encode(~key, pkcs1_digest) |> Cstruct.to_string;
};

let makeJwt = key => {
  let header = "{ \"alg\": \"RS256\" }";
  let issuedAt = Unix.time() |> int_of_float;
  let payload =
    Printf.sprintf(
      "{ \"iat\": %d, \"exp\": %d, \"iss\": 21259 }",
      issuedAt,
      issuedAt + 60 * 10,
    );
  let m = B64u.urlencode(header) ++ "." ++ B64u.urlencode(payload);
  let signature = rs256_sign(key, m) |> B64u.urlencode;
  m ++ "." ++ signature;
};
