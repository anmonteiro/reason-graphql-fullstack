open Shared.Types;

let str = ReasonReact.string;

let style = ReactDOMRe.Style.make;

module TodoItem = {
  let component = ReasonReact.statelessComponent("Todo");
  let make = (~item, ~onToggle, ~onEdit, _) => {
    ...component,
    render: _ =>
      <div
        className=Glamor.(
                    css([
                      backgroundColor("white"),
                      cursor("pointer"),
                      flexDirection("row"),
                      padding("10px"),
                      Selector(":hover", [backgroundColor("#eee")]),
                    ])
                  )
        onClick=onToggle>
        <input _type="checkbox" defaultChecked=item.completed />
        <div style=(style(~flexBasis="10px", ())) />
        <Editor value=item.title placeholder="" onChange=onEdit />
      </div>,
  };
};

let toggleItem = (item, update) =>
  Mutations.ToggleTodo.mutate({"id": item.id})
  |> Js.Promise.then_(response =>
       update(response##toggleTodo) |. Js.Promise.resolve
     );

let editItem = (item, text, update) =>
  if (text == "") {
    Mutations.RemoveTodo.mutate({"id": item.id})
    |> Js.Promise.then_(response =>
         update(response##removeTodo) |. Js.Promise.resolve
       );
  } else {
    text !== item.title ?
      Mutations.EditTodo.mutate({"title": text, "id": item.id})
      |> Js.Promise.then_(response =>
           update(response##editTodo) |. Js.Promise.resolve
         ) :
      Js.Promise.resolve();
  };

let addTodo = (update, text) =>
  Mutations.AddTodo.mutate({"title": text})
  |> Js.Promise.then_(response =>
       update(response##addTodo) |. Js.Promise.resolve
     );

module Todos = {
  module TodosQuery = [%graphql
    {|
query getTodos {
  todos @bsRecord {
    id
    title
    completed
   }
}
|}
  ];
  let query = TodosQuery.make();
  let component = ReasonReact.reducerComponent("Todos");
  type todos = array(todo);
  type data = {. "todos": todos};
  type state = todos;
  type action = state => state;
  let make = (~data, _) => {
    ...component,
    initialState: () => data##todos,
    reducer: (action, state) => ReasonReact.Update(action(state)),
    render: ({state, send}) => {
      let updateTodos = todos => send(_ => todos);
      <div>
        (
          Array.map(
            item =>
              <TodoItem
                item
                key=(item.id |> string_of_int)
                onToggle=(_ => toggleItem(item, updateTodos) |. ignore)
                onEdit=(text => editItem(item, text, updateTodos))
              />,
            state,
          )
          |> ReasonReact.array
        )
        <Editor
          value=""
          className=Glamor.(css([padding("10px 20px")]))
          onChange=(addTodo(updateTodos))
          placeholder="Add an item"
          clear=true
        />
      </div>;
    },
  };
};

module GraphQLTodos = Loader.F(Todos);

module Page = {
  let component = ReasonReact.statelessComponent("Page");
  let make = _children => {
    ...component,
    render: _ =>
      <div
        className=Glamor.(
                    css([
                      alignSelf("center"),
                      margin("50px"),
                      width("500px"),
                    ])
                  )>
        <div
          className=Glamor.(
                      css([
                        margin("20px"),
                        textAlign("center"),
                        fontSize("1.3em"),
                      ])
                    )>
          (str("Reason GraphQL Fullstack Todo List"))
        </div>
        <GraphQLTodos />
      </div>,
  };
};

ReactDOMRe.renderToElementWithId(<Page />, "index");
