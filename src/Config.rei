type t;

let get: unit => t;
let port: t => int;
let webhookSecret: t => option(string);
let appId: t => int;
