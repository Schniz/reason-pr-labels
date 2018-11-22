type state =
| Pending
| Success;
type t = {
  description: string,
  context: string,
  state,
};
let string_of_state =
fun
| Success => "success"
| Pending => "pending";
let to_json = spp =>
Ezjsonm.(
  dict([
       ("state", string_of_state(spp.state) |> string),
       ("description", string(spp.description)),
       ("context", string(spp.context)),
  ])
);

let success = {
  description: "Alright! PR has valid label(s).",
  context: "pr/labels",
  state: Success,
};
let pending = {
  description: "Don't forget to add a PR label!",
  context: "pr/labels",
  state: Pending,
};
