# phantom-call

i was setting up some infrastructure and was annoyed that i couldn't hide my active processes from standard monitoring tools without compiling a custom kernel module. so i drank a monster, ripped through the `glibc` documentation, and wrote this instead. 

this is a user-land rootkit. it compiles as a shared library (`.so`). by using the `LD_PRELOAD` environment variable, it forces the dynamic linker to load this code before standard system libraries. 

it hooks the `readdir` system call. when standard tools like `ls`, `ps`, or `top` try to list directory contents (like `/proc` for processes), this code intercept the result, dynamically filters out any file or process name that starts with "phantom_", and hands back a sanitized list. 

you are seeing what i want you to see. don't run this against other users, i wrote this for my own administrative control.

**build & deploy:**
```bash
g++ -O3 -shared -fPIC phantom_hook.cpp -o libphantom.so -ldl
export LD_PRELOAD=$PWD/libphantom.so
# now everything you run in this terminal is being filtered.
# create a file named 'phantom_test' and run 'ls'. it won't appear.
