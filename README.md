# IodineJS

### This is an old JavaScript interpreter I started many years ago.
### It was never fully completed and probably has lots of bugs.

###### Originally it was intended to be the interpreter for a browser engine I was developing from scratch.

# Features

- [x] Objects
- [x] Classes(But not static members yet.)
- [x] Functions, Anonymous functions and Arrow Functions
- [x] For and While loops
- [x] Variables
- [x] Header only.
- [x] Embeddable.

**APIs such as `JSON` are implemented in JS and actually run by the interpreter. You can look in `global.js` and see the implementations of `JSON`, `String.split`, `Array`, and more...**

# Unfinished

- [ ] Floating point numbers aren't implemented.
- [ ] Operators may be missing
- [ ] Classes aren't complete, static keyword doesn't work and extending hasn't been implemented.
- [ ] Types might not convert or behave compliant to specification.
- [ ] Memory leaks are basically guaranteed.