# cLog

cLog (or SlashET) is a simple web-based logging system written in C. It allows users to post news, comments, and manage user preferences. The project uses MySQL for database management and is built using GNU Autotools.
This was written effectively on a dare to build a basic SlashDot (or Reddit if you prefer, it didn't exist then) clone entirely in C, using only standard libraries and a database library. I wouldn't trust it to be stable, secure, or frankly relevant in this decade. It is very much a product of the late 90s. 

## Features

- Post and manage news articles
- Post and manage comments
- User authentication and session management
- RSS feed generation
- Simple HTML templating


## Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/mark-iid/clog.git
    cd clog
    ```

2. Generate the configure script and other necessary files using Automake:
    ```sh
    aclocal
    autoconf
    autoheader
    automake --add-missing
    ```

3. Configure the project:
    ```sh
    ./configure --prefix=/var/www/html --disable-dependency-tracking
    ```

4. Build the project:
    ```sh
    make
    ```

5. Install the project:
    ```sh
    sudo make install
    ```
6. Configure a mysql database and import the clog.sql file to create the tables

7. In the /var/log/www (or whereever you install it) create a text file called clog.cgi with the following contents
    ```
    database hostname
    database name
    username
    password
    ```

8. Configure your web server to protect allow execution of CGI scripts and the /logon/ directory with a authentication provider of your choice. For example:
    ```
    <Directory /var/www/>
        Options Indexes FollowSymLinks ExecCGI
        AllowOverride None
        Require all granted
        AddHandler cgi-script .cgi
        DirectoryIndex index.cgi
    </Directory>
    <Directory /var/www/html/login>
        AuthType Basic
        AuthName "Restricted Area"
        AuthUserFile /etc/apache2/.htpasswd
        Require valid-user
        AddHandler cgi-script .cgi
        DirectoryIndex login.cgi
    </Directory>
    ```

## Usage

1. Start the web server and navigate to the installation directory.
2. Access the application through your web browser.

## Contributing

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Commit your changes (`git commit -am 'Add new feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Create a new Pull Request.

## License

This project is licensed under the GNU General Public License v3.0. See the [LICENSE](http://_vscodecontentref_/32) file for details.

