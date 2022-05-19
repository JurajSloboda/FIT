import os
import click
import logging
import subprocess
import sys

from flask import Flask, render_template, redirect, url_for
from .config import Config
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from flask_bootstrap import Bootstrap
from flask_login import LoginManager
from flask_ckeditor import CKEditor

db = SQLAlchemy()
login = LoginManager()
migrate = Migrate()
bootstrap = Bootstrap()
ckeditor = CKEditor()

def create_app(test_config=None):
    # create and configure the app
    app = Flask(__name__, instance_relative_config=True)
    # app.config.from_mapping(
    #     SECRET_KEY='dev',
    #     SQLALCHEMY_DATABASE_URI=os.path.join(app.instance_path, 'app.sqlite'),
    # )
    #set config
    try:
        environment = os.environ["APP_ENVIRONMENT"]
    except KeyError:
        environment = "default"
    
    if environment == "development":
        app.config.from_object(config.DevelopmentConfig)
    elif environment == "production":
        app.config.from_object(config.ProductionConfig)
    elif environment == "default":
        app.config.from_object(config.Config)
        
    if test_config is None:
    
        # load the instance config, if it exists, when not testing
        # app.config.from_pyfile('config.py', silent=True)
        app.config.from_object(Config)
    else:
        # load the test config if passed in
        app.config.from_mapping(test_config)

    # ensure the instance folder exists
    try:
        os.makedirs(app.instance_path)
    except OSError:
        pass

    # setting up static folder
    app.static_folder = 'static'

    # register with extensions
    db.init_app(app)
    migrate.init_app(app, db)
    bootstrap.init_app(app)
    login.init_app(app)
    ckeditor.init_app(app)

    @app.cli.command('run-db-init-script')
    @click.option('-f', '--file', default='fillWithDefaultData.py')
    def runDbInitScript(file):
        subprocess.run(["python", file])

    #index page
    @app.route('/')
    @app.route('/index')
    @app.route('/home')
    def index():
        return render_template("index.html", title="Home Page")

    from .routes import auth, books, users, library, order
    app.register_blueprint(auth.bp)
    app.register_blueprint(users.bp)
    app.register_blueprint(books.bp)
    app.register_blueprint(library.bp)
    app.register_blueprint(order.bp)

    login.login_view='auth.login'
    from .models import AnonymousUser
    login.anonymous_user = AnonymousUser

    return app