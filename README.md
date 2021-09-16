# Jira Lab

A Lua-powered sandbox to perform Jira operations.

## Usage

```
jira-lab                      # Will enter interactive mode
```
Or if you want to execute a Lua script (like `luascript.lua` below):

```
cat luascript.lua | jira-lab

jira-lab < luascript.lua
```

The following environment variables *must be set*:

* `JIRA_USER`: your username on Atlassian Jira.
* `JIRA_TOKEN`: an API token.
* `JIRA_HOST`: the address of your Jira instance.

The script below will print out the keys resulting from a JQL query:

luascript.lua
```
-- List all keys of tickets created in project TEST on the last 10 days
keys = Jira.JQL ("project in (TEST) and created > -10d")

for i=1, #keys do     -- conventionally, Lua indexes starts at 1.
  print (keys[i])
end

-- List all components of project TEST
components = Jira.Components("TEST")

for i=1, #components do
  print (components[i])
end
```

## Building

### Building with Docker

To avoid installing libraries and dependencies on your machine, you can build with:

```
git clone https://github.com/johnstsimoes/jira-lab.git
cd jira-lab
docker build .
```

and to execute (use `docker image ps` to find the image ID):

```
docker run -it <build id> bash
export JIRA_USER=<your Jira user>
export JIRA_TOKEN=<your Jira API token>
export JIRA_HOST=<your Jira server address>
jira-lab
```

### Tradicional build

Ensure libraries are installed: `googletest`, `cpr`, `nlohmann-json` and `fmt`. For example, in Mac OS:

```
brew install googletest fmt cpr
brew tap nlohmann-json
brew install nlohmann-json
```

To build:

```
git clone https://github.com/johnstsimoes/jira-lab.git
cmake -S . -B build/
cmake --build build/
ctest --test-dir build/ --output-on-failure --verbose
```

To install:

```
sudo cp build/src/jira-lab /usr/local/bin
```
