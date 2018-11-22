FROM ocaml/opam2:alpine

RUN sudo apk add --no-cache ca-certificates perl gmp-dev m4

RUN sudo mkdir /app
RUN sudo chown -R $(whoami) /app
WORKDIR /app

ADD reason-pr-labels.opam /app/reason-pr-labels.opam
RUN opam install .

ADD . /app
RUN mv dune.production dune
RUN eval $(opam env) && sudo dune build Server.exe


FROM alpine
COPY --from=0 /app/_build/default/Server.exe /app
CMD /app $PORT
