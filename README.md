# CON

C Object Notation. C support for JSON

## Run Locally

```bash
  git clone https://github.com/Friedchicken-42/con con-main
  cd con-main
  make
```

## Usage/Example

### Read/Write JSON file

```c
on *o = NULL;
json_err* err = json_load("file.json");
json_dump(obj, "file.json");
```

### Create object from scratch
```c
int x = 42;

on *obj = on_create_object();
on_add(obj, "question", "What is the answer to the utlimate question?", ON_STRING);
on_add(obj, "answer", &x, ON_INTEGER);
```

### Create array from scratch
```c
on* arr = on_create_array();
on_add(arr, NULL, "test1", ON_STRING);
on_add(arr, NULL, "test2", ON_STRING);
on_add(arr, NULL, "test3", ON_STRING);
```

### Get value from a specific key
```c
char *question = on_get(obj, "question");
printf("question: %s\n", question);
```

### Free Object
```c
on_free(obj);
```

## TODO:
- [ ] add load/dump for other format
    - [ ] toml
    - [ ] xml
    - [ ] yarn
    - [ ] ini
