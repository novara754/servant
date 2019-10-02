# servant

An HTTP server for serving static files written in pure C.

# Usage

1. Use `make` to build the project.
2. Run the server using `servant <port>`.

The server will now listen on the specified port and serve files
from the `public/` directory. This directory has to be a direct child of the
current working directory.

The path of the HTTP request is appended to the path of the `public/` directory.
It has the match the file names exactly, `/` is not aliased to `/index.html`.

**Notes:** 
- Only GET requests are supported. 
- Only HTML response are supported.

## License

Licensed under the [MIT License](LICENSE.md).
