module Types = {
  type todo = {
    id: int,
    title: string,
    completed: bool
  };
  type todos = list(todo);
};
