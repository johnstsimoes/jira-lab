# Jira Lab

A Lua-powered sandbox to perform Jira operations.

## Usage

```
jira-lab                      # Will enter interactive mode
```
Or if you want to execute a Lua script (like `luascript.lua` below):

```
cat luascript.lua | jira-lab
```

The following environment variables *must be set*:

* JIRA-USER: your username on Atlassian Jira.
* JIRA-TOKEN: an API token.
* JIRA-HOST: the address of your Jira instance.

The script below will print out the keys resulting from a JQL query:

luascript.lua
```
jira = Jira.Create ("project in (TEST) and created > -10d")
print (Jira.GetKeys(jira))
```

## Building

Ensure libraries are installed: `googletest`, `cpr`, `nlohmann-json` and `fmt`. For example, in Mac OS:

```
brew install googletest fmt cpr
brew tap nlohmann-json
brew install nlohmann-json
```

To build:

```
git clone git@github.com:johnstsimoes/jira-lab.git
cmake -S . -B build/
cmake --build build/
ctest --test-dir build/ --output-on-failure --verbose
```

To install:

```
sudo cp build/src/jira-lab /usr/local/bin
```
