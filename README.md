# high-performance multithreaded http server in c

a lightweight, concurrent http server built from scratch using **posix threads** and **raw tcp sockets**. pretty straightforward stuff, just built with performance, correctness, and low-level control in mind.

this project is basically about nailing down **systems-level engineering fundamentals** — concurrency, synchronization, socket programming, and all that good stuff when it comes to handling requests efficiently.

---

## features

- **multithreaded architecture**  
  uses a custom **thread pool** to handle high concurrency without going crazy with thread creation overhead. keeps things clean.

- **low-level networking**  
  built using the native c socket api (`sys/socket.h`, `netinet/in.h`). no fancy abstraction layers, just raw networking doing its thing.

- **zero external dependencies**  
  yep, that's right. all you need is:
  - standard c library
  - posix threads (`pthread`)

- **static file serving**  
  serves files straight out of the `www/` directory. simple, no fuss.

---

## project structure

```text
my-http-server/
├── Makefile            # build automation
├── README.md           # project documentation
├── include/            # public and internal headers
│   ├── http.h
│   └── threadpool.h
├── src/                # core implementation
│   ├── main.c          # server entry point
│   ├── http.c          # http parsing and response logic
│   └── threadpool.c    # thread pool and job queue
└── www/                # static assets served by the server
    └── index.html
```

---

## build instructions

make sure you're on a posix-compliant system (linux or macos). shouldn't be a big deal.

### 1. build the server

```bash
make
```

this spits out the executable binary. that's it.

### 2. running the server

```bash
./server
```

by default, the server just listens on:

```
http://localhost:8080
```

### 3. testing

throw this into your browser:

```
http://localhost:8080
```

or if you're more of a terminal person, just hit it with `curl`:

```bash
curl http://localhost:8080
```

---

## design decisions

### thread pool

spinning up a new thread per request (`pthread_create`) doesn't really scale well under heavy load. why? well, because of:

- kernel scheduling overhead
- stack allocation cost
- context switching latency

so instead, this server runs a **fixed-size thread pool** — worker threads just hang around for the lifetime of the server. less overhead, better cache locality, and you get predictable performance even when things get busy.

### job queue

here's how it flows:

- the main thread accepts incoming client connections.
- each connection's file descriptor gets pushed into a **mutex-protected job queue**.
- worker threads chill on a **condition variable** and grab jobs whenever they're available.

pretty neat, right? this keeps things like:

- predictable latency
- bounded resource usage
- efficient cpu utilization under load

### separation of concerns

- socket handling is kept separate from request processing.
- thread management is tucked away inside the thread pool module.
- http parsing and response logic don't mix with networking code.

basically, everything's in its own lane. makes the whole codebase way easier to maintain, read, and extend down the road.

---

## goals of this project

- show off low-level systems programming in c
- get hands-on with concurrency primitives (mutexes, condition variables)
- actually understand how real-world server architecture works
- skip the unnecessary abstractions and frameworks — keep it raw

---

## future improvements

- http/1.1 persistent connections
- configurable thread pool size
- graceful shutdown handling
- basic logging infrastructure
- support for multiple mime types

---


## recommended next steps

if you're ready to take the **next serious step**, here's what to look at:

- a production-grade `Makefile`
- a short **design document** (accept → queue → worker lifecycle)
- notes on **latency, throughput, and contention points**