/* Taken from https://github.com/mmaker/ocaml-letsencrypt */

/** string head */
let head = s => s.[0];

/** string tail */
let tail = s => String.sub(s, 1, String.length(s) - 1);

let rec trim_leading_null = s =>
  if (head(s) === Char.chr(0)) {
    trim_leading_null(tail(s));
  } else {
    s;
  };

/** byte reversing */
let rev = s => {
  let rec revs = (strin, list, index) =>
    if (List.length(list) == String.length(strin)) {
      String.concat("", list);
    } else {
      revs(strin, [String.sub(strin, index, 1), ...list], index + 1);
    };
  revs(s, [], 0);
};

let urlencode = B64.encode(~pad=false, ~alphabet=B64.uri_safe_alphabet);

let urldecode = B64.decode(~alphabet=B64.uri_safe_alphabet);

let urlencodez = z => {
  let bits = Z.to_bits(z);
  rev(bits) |> trim_leading_null |> urlencode;
};

let urldecodez = z64 => {
  let bits = urldecode(z64);
  rev(bits) |> Z.of_bits;
};
