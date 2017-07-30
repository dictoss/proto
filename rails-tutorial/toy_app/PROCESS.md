# try rails tutorital

## document

- https://railstutorial.jp/


## OS environment

- Debian GNU/Linux 9 amd64
- mariadb-server-10.1
- ruby-2.3 (offical debian package)
- rails-5.1.2
- sqlite3 (offical debian package)
- mariadb-10.1 (office debian package)


## make environment

- `$ sudo apt-get install git`

- `$ sudo apt-get install ruby rubygems ruby-dev gcc g++ make`

- `$ sudo apt-get install zlib1g-dev sqlite3 libsqlite3-dev`

- `$ sudo apt-get install mariadb-server mariadb-client libmariadb-dev`

- `$ sudo gem install rails -v 5.1.2`


## generate rails application template

- `$ mkdir path/to/dir`

- `$ cd path/to/dir`

- `$ rails _5.1.2_ new toy_app -d mysql`

  - many install gems.

- `$ ls toy_app`


## exec develop httpd server

- `$ cd toy_app`

- modify Gemfile.

  - therubyracer

    - # gem 'therubyracer', platforms: :ruby
    - gem 'therubyracer', platforms: :ruby

  - mariadb-client

    - gem 'mysql2'

- `$ sudo bundle install`

- `$ rails s`

- `$ rails s -b 0.0.0.0`

  - => Booting Puma
  - => Rails 5.1.2 application starting in development on http://0.0.0.0:3000
  - => Run `rails server -h` for more startup options

- access web browser, 'http://youripaddress:3000'

  - Yay! You’re on Rails!


## mariadb connection setting

- `$ sudo mysql_secure_installation`

- `$ sudo mysql`

   - `MariaDB [(none)]> CREATE DATABASE toy_app DEFAULT CHARACTER SET utf8;`
   - `MariaDB [(none)]> CREATE DATABASE toy_app_devel DEFAULT CHARACTER SET utf8;`
   - `MariaDB [(none)]> GRANT ALL ON toy_app_devel.* TO 'webapp'@'localhost' IDENTIFIED BY 'password';`
   - `MariaDB [(none)]> FLUSH PRIVILEGES;`
   - `MariaDB [(none)]> exit`

- test connect mysql.

  - `$ mysql -u webapp -p toy_app`


## go tutorial

- `$ vi app/controllers/application_controller.rb`

- `$ vi config/routes.rb`

## generate model

- `$ cd toy_app`

- set config "User" schema.

  - `$ rails generate scaffold User name:string email:string`
  - auto generate id column.
  - Q. set bingit type for id ?

    - A. If rails-5.1.2, id type is bigint on mariadb-10.1 .

- change database config.

  - want to mariadb.
  -`$ vi config/database.yml`

- migrate database

  - `$ rails db:migrate`

- show user management page.

  - exec `$ rails s -b 0.0.0.0`

  - access webpage "http://yourhost:3000/users/"

  - create user.

    - username: testuser1
    - Email: testuser1@example.com


