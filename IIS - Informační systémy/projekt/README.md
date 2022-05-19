# IIS project
# Requirements
All system requiremetns in requirements.txt
run: pip install -r requirements.txt in your shell

# Instalation
## Set environment variables


set the following environment variables

+ FLASK_APP : must be 'app'
+ FLASK_ENV : must be 'development'
+ APP_ENVIRONMENT : must be 'development'
+ SECRET_KEY
+ DATABASE_URL : uri of the database. Can be an absolute path to sqlite db (sqlite:///path/to/db)

## Migrate database
```
flask db init
flask db migrate
flask db upgrade
```

## Start server
```
flask run
```