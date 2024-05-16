# OOP HW2-3 Fix

This repository attempts to modernize JJK's code.

## Breaking Changes

In order to improve on JJK's code, the following breaking changes have been made:

- All mutators and accessors have been renamed to use snake_case instead of camelCase.

- Since all `generator`s are inner classes of the classes they generate objects for, their names don't need to include their enclosing classes' names again, so they have all been renamed to `generator`.
