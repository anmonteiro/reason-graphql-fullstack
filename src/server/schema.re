open Shared.Types;

module GQL = Graphql_lwt;

type appState = {
  nextId: int,
  todos,
};

let appState =
  ref({
    nextId: 1,
    todos: [{id: 0, title: "Add some things to do", completed: false}],
  });

let todo =
  GQL.Schema.(
    obj("todo", ~fields=_ =>
      [
        field("id", ~args=Arg.[], ~typ=non_null(int), ~resolve=({ ctx : ()}, p) =>
          p.id
        ),
        field("title", ~args=Arg.[], ~typ=non_null(string), ~resolve=(_ctx, p) =>
          p.title
        ),
        field(
          "completed", ~args=Arg.[], ~typ=non_null(bool), ~resolve=(_ctx, p) =>
          p.completed
        ),
      ]
    )
  );

let schema =
  GQL.Schema.(
    schema(
      [
        io_field(
          "todos",
          ~args=Arg.[],
          ~typ=non_null(list(non_null(todo))),
          ~resolve=(_ctx, ())
          /* Hack: reverse the list because we insert new todos at the head */
          => Lwt_result.return(appState^.todos |> List.rev)),
      ],
      ~mutations=[
        io_field(
          "addTodo",
          ~typ=non_null(list(non_null(todo))),
          ~args=
            Arg.[
              arg("title", ~typ=non_null(string)),
              arg("completed", ~typ=non_null(bool)),
            ],
          ~resolve=(_, _, title, completed) => {
            let newId = appState^.nextId;
            let newTodo = {id: newId, title, completed};
            appState :=
              {nextId: newId + 1, todos: [newTodo, ...appState^.todos]};
            Lwt_result.return(appState^.todos |> List.rev);
          },
        ),
        io_field(
          "toggleTodo",
          ~typ=non_null(list(non_null(todo))),
          ~args=Arg.[arg("id", ~typ=non_null(int))],
          ~resolve=(_, _, idToToggle) => {
            appState :=
              {
                ...appState^,
                todos:
                  List.map(
                    ({id, completed} as todo) =>
                      if (id == idToToggle) {
                        {...todo, completed: !completed};
                      } else {
                        todo;
                      },
                    appState^.todos,
                  ),
              };

            Lwt_result.return(appState^.todos |> List.rev);
          },
        ),
        io_field(
          "editTodo",
          ~typ=non_null(list(non_null(todo))),
          ~args=
            Arg.[
              arg("id", ~typ=non_null(int)),
              arg("title", ~typ=non_null(string)),
            ],
          ~resolve=(_, _, idToEdit, title) => {
            appState :=
              {
                ...appState^,
                todos:
                  List.map(
                    ({id} as todo) =>
                      if (id == idToEdit) {
                        {...todo, title};
                      } else {
                        todo;
                      },
                    appState^.todos,
                  ),
              };
            Lwt_result.return(appState^.todos |> List.rev);
          },
        ),
        io_field(
          "removeTodo",
          ~typ=non_null(list(non_null(todo))),
          ~args=Arg.[arg("id", ~typ=non_null(int))],
          ~resolve=(_, _, idToRemove) => {
            appState :=
              {
                ...appState^,
                todos:
                  List.filter(({id}) => id !== idToRemove, appState^.todos),
              };
            Lwt_result.return(appState^.todos |> List.rev);
          },
        ),
      ],
    )
  );
