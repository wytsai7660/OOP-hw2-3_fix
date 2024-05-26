# OOP HW2-3 Fix

This repository attempts to modernize JJK's code.

![UML](assets/UML.svg)

## Breaking Changes

In order to improve on JJK's code, the following breaking changes have been made(might not be exhaustive):

- All mutators and accessors have been renamed to use snake_case instead of camelCase.
- `GET` and `SET` has been modified to take one parameter only. The type of the property, the name of the accessor/mutator and the name of their parameters will be generated automatically. `GET_WITH_NAME` and `SET_WITH_NAME` were also added for customizing the generated accessor/mutator names.
- All generators have been deleted. Templates are used in place of them for a more type-safe approach.
- All special members have become public because of the removal of generators.
- `*::generator::print` functions have been moved to their enclosing classes of the inner classes they belong. For example, call `header::print` instead of `header::generator:print`. Except for `event::generator::print`, which have been moved to `event::print_registered_event_types` because there was already an `event::print` that did something different.
- `packet::discard` has been removed since none of its members are dynamically allocated anymore.

## Usage Guides

This is a major revamp of JJK's code, so there are some features that you need to access in a different way. The following list tries to list all of them.

- Instead of calling `IoT_ctrl_payload::increase` directly, call `IoT_ctrl_packet::increase_payload_counter`.
