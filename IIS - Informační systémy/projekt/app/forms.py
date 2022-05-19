from logging import PlaceHolder
from flask_wtf import FlaskForm
from flask_ckeditor import CKEditorField
from wtforms import StringField, PasswordField, BooleanField, SubmitField, ValidationError, TextAreaField, SelectField, TimeField
from wtforms.fields import IntegerField
from wtforms.validators import DataRequired, EqualTo, Length

from app.models import *
from app.enums import UserStatus

class LoginForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired()])
    password = PasswordField('Password', validators=[DataRequired()])
    remember_me = BooleanField('Remember Me')
    submit = SubmitField('Sign In')

class RegisterForm(FlaskForm):
    username = StringField('Username', validators=[DataRequired()])
    password = PasswordField('Password', validators=[DataRequired()])
    password2 = PasswordField('Repeat Password', validators=[DataRequired(), EqualTo('password')])
    submit = SubmitField('Register')

    def validate_username(self, username):
        user = User.query.filter_by(username=username.data).first()
        if user is not None:
            raise ValidationError('Username already in use.')

class ProfileEditForm(FlaskForm):
    #profile description
    description = CKEditorField('description')
    profileStatus = SelectField('Profile Status', choices=UserStatus)

    submitProfileEdit = SubmitField('Submit profile edit')

class AdminProfileEditForm(FlaskForm):
    #profile description
    description = CKEditorField('description')
    profileStatus = SelectField('Profile Status', choices=UserStatus)

    submitProfileEdit = SubmitField('Submit profile edit')

class PasswordChangeForm(FlaskForm):
    #passwords
    oldPassword = PasswordField('Old Password', validators=[DataRequired()])
    newPassword = PasswordField('New Password', validators=[DataRequired()])
    newPassword2 = PasswordField('Repeat new password', validators=[DataRequired(), EqualTo('newPassword')])

    submitPasswordChange = SubmitField('Change Password')

class AddBookForm(FlaskForm):
    name = StringField('Book Name', validators=[DataRequired(), Length(min=0, max=160)])
    description = TextAreaField('Book Description')
    tags = TextAreaField('Tags')
    authors = TextAreaField('Authors')
    isbn = StringField('Isbn', validators=[ Length(min=0, max=160)])
    year = IntegerField('Release year')
    rating = IntegerField('Rating')
    publisher = StringField('Publisher', validators=[ Length(min=0, max=160)])

    submit = SubmitField('Add Book')

class AddLibraryForm(FlaskForm):
    name = StringField('Library Name', validators=[DataRequired(), Length(min=0, max=160)])
    description = TextAreaField('Library Description')
    address = StringField('Address', validators=[ Length(min=0, max=160)])
    openTime = IntegerField('Opening hour')
    closeTime = IntegerField('Closing hour')
    submit = SubmitField('Add Library')
    
class LibraryEditForm(FlaskForm):
    name = StringField('Library Name', validators=[DataRequired(), Length(min=0, max=160)])
    description = TextAreaField('Library Description')
    address = StringField('Address', validators=[ Length(min=0, max=160)])
    openTime = IntegerField('Opening hour')
    closeTime = IntegerField('Closing hour')

    submitProfileEdit = SubmitField('Submit Library edit')


class SearchBooksByTagsOrNameForm(FlaskForm):
    searchBar = TextAreaField('Search by Names or Tags (separated by comma)')

    submit = SubmitField('Search')

class OrderForm(FlaskForm):
    amount = IntegerField('Amount')
    submit = SubmitField('Order amount for Library')

class SearchLibsByNameForm(FlaskForm):
    searchBar = TextAreaField('Search by Names')

    submit = SubmitField('Search')

class reservationForBook(FlaskForm):
    submit = SubmitField("Reserve Book")

class addLibrarianToLibrary:
    submit = SubmitField("add Librarian")

class BookEditForm(FlaskForm):
    name = StringField('Book Name', validators=[ Length(min=0, max=160)])
    description = TextAreaField('Book Description')
    authors = TextAreaField('Authors')
    isbn = StringField('Isbn', validators=[ Length(min=0, max=160)])
    year = IntegerField('Release year')
    rating = IntegerField('Rating')
    publisher = StringField('Publisher', validators=[ Length(min=0, max=160)])

    submitProfileEdit = SubmitField('Submit Book edit')
