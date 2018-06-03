open Shared.Types;

module type MutationType = {
  type t;
  type variables;

  module Mutation: {
    let makeWithVariables:
      variables =>
      {
        .
        "parse": Js.Json.t => t,
        "query": string,
        "variables": Js.Json.t,
      };
  };
};

module CreateMutation = (Config: MutationType) => {
  let mutate = variables => {
    let mutation = Config.Mutation.makeWithVariables(variables);
    Loader.sendQuery(mutation);
  };
};

module AddTodoMutation = {
  module Mutation = [%graphql
    {|
   mutation addTodo($title: String!) {
     addTodo(title: $title, completed: false) @bsRecord {
       id
       title
       completed
     }
   }
|}
  ];
  type t = {. "addTodo": Js.Array.t(Shared.Types.todo)};
  type variables = {. "title": string};
};

module AddTodo = CreateMutation(AddTodoMutation);

module ToggleTodoMutation = {
  module Mutation = [%graphql
    {|
   mutation toggleTodo($id: Int!) {
     toggleTodo(id: $id) @bsRecord {
       id
       title
       completed
     }
   }
|}
  ];
  type t = {. "toggleTodo": Js.Array.t(Shared.Types.todo)};
  type variables = {. "id": int};
};

module ToggleTodo = CreateMutation(ToggleTodoMutation);

module EditTodoMutation = {
  module Mutation = [%graphql
    {|
   mutation editTodo($id: Int!, $title: String!) {
     editTodo(id: $id, title: $title) @bsRecord {
       id
       title
       completed
     }
   }
|}
  ];
  type t = {. "editTodo": Js.Array.t(Shared.Types.todo)};
  type variables = {
    .
    "title": string,
    "id": int,
  };
};

module EditTodo = CreateMutation(EditTodoMutation);

module RemoveTodoMutation = {
  module Mutation = [%graphql
    {|
   mutation removeTodo($id: Int!) {
     removeTodo(id: $id) @bsRecord {
       id
       title
       completed
     }
   }
|}
  ];
  type t = {. "removeTodo": Js.Array.t(Shared.Types.todo)};
  type variables = {. "id": int};
};

module RemoveTodo = CreateMutation(RemoveTodoMutation);
