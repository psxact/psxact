# Welcome

Welcome to the PSXACT project.

The goal of the project is to create the best PS1 emulator possible, we believe
that meeting this goal requires more than just being able to boot every game or
pass every test. Those are nice goals, to be sure, but being able to document
how the system with clean code is also nice. What do we mean by clean code?
Read on to find out!

## Portability

Portable code tends to be cleaner than system-specific code, at least, it is in
our experience. Does this mean that system-specific code is banished? Of course
not! It just means that it should be constrained, and kept out of the core where
it doesn't belong. Also, if you are designing system-specific code for one
system please do us a favor and create fall-backs for other systems so no
compatibility gaps occur.

## Readability

Readable code is another imperative. The PS1 is a pretty complex piece of
hardware and emulating it is no easy feat. The last thing a project of this
complexity needs is to have code that is hard to follow (the system makes that
inevitable already).

### Style

- We use spaces over tabs for indentation with one indent being equivalent to
  two spaces.

- `if` statements must always come with braces, as do `else` statements.
  - The opening brace will be on the same line as the statement.
  - The content inside of the braces will be on its own line, even for single
    statements.
  - Closing braces will be on their own line as well.
  - Example:
    ```
    if (some_condition) {
      printf("Some condition was true\n");
    }
    ```

- `switch` statements follow the same spirit as `if`s.
  - `case` labels will be on their own line.
  - Code inside of a `case` will be surrounded in braces only if local variables
    are declared (the code won't compile otherwise).
  - Falling from one case to another is considered an typo error.

- Identifiers are in `all_lower_case` with underscores to separate words. Lower
  casing applies to initialisms and acronyms as well.

- Loops (`while`, `for`, `do`) follow the same brace rules as `if` statements.

## Testing

A testing repository is in the works. Once it is established we will require
any changes to be run through the tests, to ensure quality is preserved. All
of the tests will also be verified against actual hardware, currently we have
the following systems available for testing:

- SCPH-1001 (US)
- SCPH-5501 (US)
- SCPH-7501 (US)

If you have other systems, are able to run custom code on them, and are
interested in helping to expand our testing platform please don't hesistate to
contact us!

Having strong test cases to validate against is also helpful, if you'd like to
contribute on that front then
[please do](https://github.com/adam-becker/teststation])!
