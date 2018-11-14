# Reason / GraphQL Shared Backend

This is an example of a fullstack Reason + GraphQL application that uses a native
backend and a client compiled to JavaScript through BuckleScript, whilst sharing
types between the backend and the frontend.

<img src="./demo.gif">

## Technologies / Libraries:

This was initially based on [@jaredly](https://github.com/jaredly)'s
[isomagic-todos](https://github.com/jaredly/isomagic-todos) experiment that accomplished
sharing types between the backend & frontend through some PPX magic.

In this case, we leverage the power of GraphQL and its support in OCaml through
[ocaml-graphql-server](https://github.com/andreas/ocaml-graphql-server) and
[graphql_ppx](https://github.com/mhallin/graphql_ppx) to build our API through shared
OCaml / Reason types (have a look in [`src/shared`](./src/shared)).

## Running

### Build the front-end

- `yarn && yarn build && yarn bundle`

### Build and run the backend

**Note**: _it's a quiet startup; check http://localhost:8080/graphql to see that it's running!_

- `npm install -g esy`
- `esy @server`
- `esy @server x graphql`

## Neat tricks

- GraphQL schema fully defined in Reason (see [`src/server/schema.re`](./src/server/schema.re))
- There's also a GraphiQL instance at `localhost:8080/graphql` for exploring and
playing with the queries and mutations.

## License

MIT
