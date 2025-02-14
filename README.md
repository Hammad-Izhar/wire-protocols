# Sock-et Out
## COMPSCI 2620 Design Exercise 1: Wire Protocols
### Group 36: Hammad Izhar, Thomas Kaminsky

Below is a summary of the important components of Sock-et Out, implemented as part of CS 2620: Distributed Computing.

Sock-et Out is an SMS-like chat service enabling users to communicate with one another in real-time private channels. The host server enables users to communicate across devices connected via non-protected wifi networks to create, exchange, and delete channels and messages in real-time. The project is designed to be highly modular&mdash;adding new functionality simply requires implementing the following:

1. Request message classes from `client -> server` and `server -> client` specifying data to transmit between client and server (and a serialization scheme for the payload, if not using JSON).
2. Message handler functions in `bin/client/model/message_handlers.cpp/hpp` and `bin/server/model/message_handlers.cpp` covering logic for the task&mdash;usually interfacing with the database.
3. Qt event detection for sending each message and updating the client's GUI.

The backend is designed to offer maximum flexibility for creating new features using existing infrastructure. Abstractions given by the `User`, `Message`, `Channel`, `UUID`, and `Database` classes offer flexibility for accessing relevant data, even for unanticipated services. For example, multi-user chats, currently unsupported on the app, will require no backend changes, as `Channel`s already store arbitrary user groups, and the `Database` handles logic for adding and removing members from groups of arbitrary sizes.

For a summary of the relevant components of the project, see the documentation below.

## Table of Contents

### [Setup and Running](#setup-and-running-1)

### [Overview of Functionality](#overview-of-functionality-1)

### [Classes](#classes-1)
* **[UUID](#uuid)**
* **[Snowflake](#snowflake)**
* **[User](#user)**
* **[Message](#message)**
* **[Channel](#channel)**

### [Data Management](#data-management-1)
* **[Database](#database)**
* **[Message Table](#message-table)**
* **[Channel Table](#channel-table)**
* **[Password Table](#password-table)**
* **[User Table](#user-table)**

### [Request Messages](#request-messages-1)
* **[Header](#header)**
* **[Register Account](#register-account)**
* **[Login](#login)**
* **[List Accounts](#list-accounts)**
* **[Create Channel](#create-channel)**
* **[Send Message](#send-message)**
* **[Delete Message](#delete-message)**
* **[Delete Account](#delete-account)**









### [Server](#server)








# Setup and Running

To run the code, you'll need [Docker](https://www.docker.com/) and an X11 server. On Linux, you most likely already have an X11 server installed. On Mac/Windows consider using [XQuartz](https://www.xquartz.org/) (Mac) or [Xming](https://sourceforge.net/projects/xming/) (Windows), respectively.

**NOTE** This code has only been verified on Mac, and briefly tested on Linux. Your mileage may very.

>[!INFO]
> On Linux, `docker.host.internal` is not natively supported. Therefore, it is recommended to first verify the value of `$DISPLAY` locally and then export it in the docker container. Also be sure to verify xhost connections are allowed by `xhost +local:`

## Setup Steps (Mac)

Open XQuartz. In 'XQuartz' settings, navigate to Settings -> Security -> 'Allow connections from network clients', and then restart your machine.

Open a terminal, and allow xhost connections by running the following:

```
xhost +local:
```

Next, clone the repo and build and enter the Docker container:

```bash
git clone https://github.com/Hammad-Izhar/wire-protocols && cd wire-protocols
docker compose up --build -d
docker attach wire-protocols-app-1 # replace this with the container name
```

To build the code, navigate to the build directory and run the following commands:

```
cd ~/cs2620/wire-protocols/build
cmake ..
make
```

If you're getting error `Authorization required, but no authorization protocol specified`, open a new terminal and run

```
xhost +local:
```

## Running the Code

Once installation is complete, you can start a server by running

```
./server
```

**in the `/build` directory.**

Likewise, run

```
./client
```

to spin up a new client, and

```
./test
```

to run unit tests.



# Overview of Functionality

Currently, our application supports the following functionality:

* Registering new users, ensuring no duplicate usernames.
* Logging in to the app
* Searching for users by regex
* Creating new chat channels (between two users)
* Sending and receiving messages in real-time
* Deleting messages from channels
* Logging out of the application (user data persists while server remains running)
* Deleting user accounts

## Future Functionality

The following functionality is supported on the backend, and we would like to integrate it into the frontend in the future:

* Editing messages
* Choosing and modifying profile pictures
* Modifying channel names
* Modifying usernames
* Notating read by and modified at indicators/timestamps
* Creating many-person (>2) channels
* Adding and removing users from channels

## File Structure

Our project has the following structure (tree shown below). Headers are all in `/include`, implementations are in `src/`, and unit tests are in `test/`. 

Client and server information is separated into `client/` and `server/` subdirectories, with shared classes outside of these directories.

```
.
├── assets
│   └── [Files including profile pictures, loading gifs, etc.]
├── include
|   ├── [Contains headers for all classes.]
│   ├── client
│   │   ├── gui
│   │   │   └── [Files for displaying the app]
│   │   └── model
│   │       └── [General classes for the client]
│   ├── server
│   │   ├── db
│   │   │   └── [Classes for data storage and access]
│   │   └── model
│   │       └── [Classes for server-side message-passing]
│   ├── message
│   │   └── [Shared classes for sending messages between client and server]
│   ├── models
│   │   └── [Shared classes for comprehending data]
│   ├── constants.hpp [Relevant constants]
│   └── json.hpp [For JSON serialization/deserialization]
├── src
|   ├── [Contains implementations for all classes.]
│   ├── bin
│   │   ├── client
│   │   │   ├── gui
│   │   │   │   └── [Files for displaying the app]
│   │   │   ├── model
│   │   │   │   └── [General classes for the client]
│   │   │   └── main.cpp [Main file for running client]
│   │   └── server
│   │       ├── db
│   │       │   └── [Classes for data storage and access]
│   │       ├── model
│   │       │   └── [Classes for server-side message-passing]
│   │       └── main.cpp [Main file for running server]
│   ├── message
│   │   └── [Classes for sending messages between client and server]
│   └── models
│       └── [General-use classes for comprehending data]
└── test
    ├── main.cpp [Main file for running tests]
    ├── database
    │   └── [Tests for manipulating data in the tables and database]
    ├── message
    │   └── [Tests for sending/receiving messages]
    └── models
        └── [Tests for individual backend components]
```


# Classes

Below we describe the classes used for comprehending messages, users, and channels. 

In general, class headers are initialized in the `/include/` directory, and implementations in the `/src/` directory. The full project structure is outlined below:


## UUID

To accomodate a large user base, we created a class for unique user IDs to easily store large integers as an `std::array<uint8_t, 16>`. Values are either randomly generated on initialization or pre-populated. Otherwise, it's pretty spare&mdash;methods to serialize/deserialize UUIDs, perform numeric comparisons, and so on.

## Snowflake

Snowflake is a utility class for generating unique snowflakes for each message. See `src/models/snowflake.cpp` and `src/models/message.cpp` for details.

## User

`User`s share important profile information for individuals signed up for the service.


* `UUID uid`: The unique user ID used for referencing users in the database
* `std::string username`: Their username, for login.
* `std::string display_name`: Their display name for messages.
* `std::string profile_pic`: A link to the user's profile picture.
* `std::string public_key`: The user's public key (for message encryption).
* `std::vector<UUID> channels`: A vector of UUIDs referencing all channels of which the user is a part.

Otherwise, methods are getters/setters for the relevant quantities above. 

## Message

`Message`s contain user communications, as well as related metadata:

* `uint64_t snowflake`: Unique ID for each message.
* `UUID sender_id`: ID of message sender.
* `UUID channel_id`: ID of the channel that the message was sent in.
* `uint64_t created_at`: Time of message creation.
* `uint64_t modified_at`: Time that the message was last modified.
* `std::vector<UUID> read_by`: A list of all users who have read the message.
* `std::string text`: The message payload.



## Channel

Each `Channel` stores references to all users and messages that it contains, as well as chanel metadata:

* `UUID uid`: The channel's unique identifier, for referencing it.
* `std::vector<UUID> user_uids`: A list of IDs corresponding to all users belonging to the channel.
* `std::vector<uint64_t> message_snowflakes`: Message snowflakes corresponding to each message in the channel.

Methods are just serialize/deserialize and relevant getters/setters.

# Data Management

Below we describe the important methods for managing our app's data, implemented in `server/db`.

Abstractly, all data access and modification is mediated by the `Database`, which consists of four tables: one mapping from user UIDs to passwords, one from user UIDs to user objects, one from Channel UIDs to channels, and one from message snowflakes to messages. See details below.

## Database

Our `Database` is structured somewhat like a poor man's SQL database. It serves as the point of entry for any updates to channel state, including adding/removing users, messages, and channels, creating and logging into accounts, and searching for users by regex. It's composed of four dictionaries:

* `unique_ptr<UserTable> users`: The user table.
* `unique_ptr<MessageTable> messages`: The message table.
* `unique_ptr<ChannelTable> channels`: The channel table.
* `unique_ptr<PasswordTable passwords`: The password table.

The database is responsible for implementing higher-order operations between these tables; e.g. when removing a user, we must remove the user from the user table, remove all of their messages from the message table, remove them from each of their channels, and delete the channel if they were the only member. 

## Message Table

The message table is comprised of an `std::unordered_map<uint64_t, Message::SharedPtr> data` containing all messages, referenced by snowflake. Functionality consists of simple getters/setters.

## User Table

Like the message table, the user table is an ` std::unordered_map<UUID, User::SharedPtr>` of users which implements basic getters/setters. The only additions are two methods:

* `std::vector<UUID> get_uuids_matching_regex(std::string regex)`: Grabs the IDs of all users whose username matches a properly-formatted regex (for name lookup).
* `std::optional<UUID> get_uid_from_username(std::string username)`: Gets the UUID associated with a username, if it exists. Used for verifying that usernames are unique during registration.

## Channel Table

Another dictionary from IDs to channels (`std::unordered_map<UUID, Channel::SharedPtr>`) implementing getters/setters.

## Password Table

Like the other tables, a `std::unordered_map<UUID, std::pair<std::string, std::string>>`. This one maps from user IDs to hashed passwords and a string corresponding to [salt](https://en.wikipedia.org/wiki/Salt_(cryptography)), used to make the hash more secure. Includes methods to add/remove passwords, as well as a function `std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password)` which checks whether a user has supplied the right password to log in.



# Request Messages 

This section describes the messages sent between client and server to control make requests for each database action.

All messages implement serialize / deserialize, both with JSON and our custom serialization scheme.

Each message is routed to a separate handler in `client/models/message_handlers.hpp/cpp` or `server/models/message_handlers.hpp/cpp`, depending on whether it is `server -> client` or `client -> server`.

## Header

The header is prepended to all messages, and it specifies metadata:

*  `uint8_t version`: Currently, 1.
* `enum Operation operation`: The message being sent (see the `enum` in `include/message/header.hpp`)
* `uint16_t packet_length`: Size of the payload.

## Register Account

`Client -> Server`

Sends username, password, and display name.

Handler tries to create an account, with success if username is not taken and password is valid.

**Response**

`Server -> Client`

Sends server response back to client.

Handler either redirects to login (on success) or notes an error during account creation.

## Login

`Client -> Server`

Sends username and password. 

Handler checks if client can accept new logins, and, if so, checks user credentials. On success, the client authenticates a user (setting a flag that the user is logged in). Otherwise, login fails.

**Response**

`Server -> Client`

Sends server response. Transitions to logged-in screen if successful.

## List Accounts

`Client -> Server`

Sends a regex for pattern-matching users.

Handler grabs all accounts matching the regex by UUID, and sends copies of the users back to the client.

**Response**

`Server -> Client`

Response sends the users back to client, handling when regex fails to receive any.

## Create Channel

`Client -> Server`

Sends the channel name and a list of initial member IDs. 

Handler creates the channel and returns a pointer to it if possible.

**Response**

`Server -> Client`

Return message sends a shared pointer to the channel or shares the error.

## Send Message

`Client -> Server`

Sends a channel ID, sender ID, and message text to the server to add a message.

**Response**

`Server -> Client`

Returns either a pointer to the created message, or an error string in case of failure.

## Delete Message

`Client -> Server`

Sends the channel ID and message snowflake to be deleted.

**Response**

`Server -> Client`

Returns either the removed message's snowflake or an error string.

## Delete Account

`Client -> Server`

Sends the user ID to be deleted.

**Response**

`Server -> Client`

Returns either the removed user's ID or an error string.


# Server

















