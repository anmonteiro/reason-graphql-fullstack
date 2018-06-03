let style = ReactDOMRe.Style.make;

let evtValue = event => ReactDOMRe.domElementToObj(
                          ReactEventRe.Form.target(event),
                        )##value;

type retainedProps = string;

let component = ReasonReact.reducerComponentWithRetainedProps("Editor");

let make = (~value, ~onChange, ~placeholder, ~className="", ~clear=false, _) => {
  ...component,
  initialState: () => None,
  reducer: (action, state) => ReasonReact.Update(action(state)),
  retainedProps: value,
  willReceiveProps: ({state, retainedProps}) =>
    switch (state) {
    | None => None
    | Some(text) =>
      retainedProps === value && value !== text ? Some(text) : None
    },
  render: ({state, send}) =>
    switch (state) {
    | None =>
      <div
        style=(style(~cursor="text", ()))
        className
        onClick=(
          evt => {
            ReactEventRe.Mouse.stopPropagation(evt);
            send(_ => Some(value));
          }
        )>
        (ReasonReact.string(value === "" ? placeholder : value))
      </div>
    | Some(text) =>
      <input
        value=text
        placeholder
        className
        autoFocus=true
        onChange=(
          evt => {
            let value = evtValue(evt);
            send(_ => Some(value));
          }
        )
        onClick=(evt => ReactEventRe.Mouse.stopPropagation(evt))
        onKeyDown=(
          evt =>
            switch (ReactEventRe.Keyboard.key(evt)) {
            | "Enter" =>
              if (text == value) {
                send(_ => None);
              } else {
                onChange(text);
                if (clear) {
                  send(_ => Some(value));
                };
              }
            | _ => ()
            }
        )
        style=(
          style(~fontFamily="inherit", ~flex="1", ~fontSize="inherit", ())
        )
        onBlur=(
          _ =>
            if (text != value) {
              onChange(text);
              if (clear) {
                send(_ => Some(value));
              };
            } else {
              send(_ => None);
            }
        )
      />
    },
};
