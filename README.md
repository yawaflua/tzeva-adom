# Linux app for tzeva-adom and OREF.

This application can show a notification with sound to the PC user when tzeva-adom and oref are running.

# Working
## Starting:
For start this app, you should to make this actions:
- Build the app:
  - ```bash
    cmake .
    make
    ```
- Start the app:
  - ```bash
      cd cmake-build-debug
      ./tzeva-adom --lang en --test
    ```
## Command args:

| flag     | description                                                    | value      |
|----------|----------------------------------------------------------------|------------|
| --lang   | Changes the language of alerts                                 | `en/he/ru` |
| -l       | Alias of `--lang`                                              | `en/he/ru` |
| --test   | Creates a test alert on start, after it working in normal mode | none       |
| -t       | Alias of `--test`                                              | none       |
| --debug  | Show debug data about alerts                                   | none       |
| -d       | Alias of `--debug`                                             | none       |
| -o       | Alias of `--oref`                                              | none       |
 | --oref   | Use OREF api, not tzeva_adom                                  | none       |
