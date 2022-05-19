import click
import logging
from flask import Blueprint, render_template, request, flash, Response, url_for, redirect
from flask_login import login_required, current_user

from app.models import BookCopy, User, VoteForBook, makeUserAdmin, Library, LibraryToLibrarian, Order
from app.forms import ProfileEditForm, PasswordChangeForm
from app import db
from app.enums import UserStatus

bp = Blueprint('users', __name__, url_prefix="/user")

@bp.route('/__all')
def allUsers():
    users = User.query.all()
    return render_template('users/allUsers.html', title="All Users", users = users)

@bp.route('/<userId>/profile')
def userProfile(userId):
    #get the user if it exists
    user = User.query.filter_by(id=userId).first_or_404()

    return render_template('users/userProfile.html', title="User Profile", user = user)

@bp.route('/<username>/profile/edit/editProfile', methods=["GET","POST"])
@login_required
def editUserProfile(username):
    profileEditForm = ProfileEditForm()
    passwordChangeForm = PasswordChangeForm()
    user = User.query.filter_by(username=username).first_or_404()

    if profileEditForm.validate_on_submit():
        user.profile_desc = profileEditForm.description.data
        
        if(profileEditForm.profileStatus.data == "UserStatus.ADMIN"):
            user.setAdmin()
        if(profileEditForm.profileStatus.data == "UserStatus.USER"):
            user.setUser()
        if(profileEditForm.profileStatus.data == "UserStatus.LIBRARIAN"):
            user.setLibrarian()
        if(profileEditForm.profileStatus.data == "UserStatus.DISTRIBUTOR"):
            user.setDistributor()

        db.session().commit()
        flash("Changes saved")

    elif request.method == "GET":
        profileEditForm.description.data = user.profile_desc
        # profileEditForm.profileVisibility.data = current_user.profile_visibility

    return render_template('users/userProfileEdit.html', title="User Profile Edit", profileEditForm=profileEditForm, passwordChangeForm=passwordChangeForm)

@bp.route('/<username>/profile/edit/changePassword', methods=["GET","POST"])
@login_required
def changePassword(username):
    profileEditForm = ProfileEditForm()
    passwordChangeForm = PasswordChangeForm()

    user = User.query.filter_by(username=username).first_or_404()
    print(passwordChangeForm.is_submitted())
    if passwordChangeForm.validate_on_submit():
        #validate password
        flash("Changing Password")
        if not user.check_password(passwordChangeForm.oldPassword.data):
            flash("Invalid password.")
        else:
            user.set_password(passwordChangeForm.newPassword.data)
            db.session().commit()
            flash("Password changed")
    elif request.method == "GET":
        profileEditForm.description.data = user.profile_desc

    return render_template('users/changePassword.html', title="User Profile Edit", profileEditForm=profileEditForm, passwordChangeForm=passwordChangeForm)

@bp.route('<username>/delete')
def deleteUser(username):
    user = User.query.filter_by(username=username).first()
    librarian = LibraryToLibrarian.query.filter_by(librarianId=user.id)
    order = Order.query.filter_by(orderUserId=user.id)
    user = User.query.filter_by(username=username)
    librarian.delete()
    order.delete()
    user.delete()
    db.session.commit()
    logging.info(f"Deleted user {username}.")
    return redirect(url_for('index'))

@bp.route('<userId>/addLibrarianToLibrary', methods=["GET","POST"])
@login_required
def addLibrarianToLibrary(userId):
    librarian = User.query.filter_by(id=userId).first_or_404()
    libraries = Library.query
    return render_template('users/addLibrarianToLibrary.html', title="Add Librarian To Library", user = librarian, libraries=libraries)

@bp.route('<userId>/<libraryId>/addLibrarianToLibraryDone', methods=["GET","POST"])
@login_required
def addLibrarianToLibraryDone(userId,libraryId):
    librarytolibrarian = LibraryToLibrarian.query.filter_by(librarianId=userId)
    librarytolibrarian.delete()
    librarytolibrarian = LibraryToLibrarian(
        libraryId=libraryId,
        librarianId=userId
    )
    db.session.add(librarytolibrarian)
    db.session.commit()
    return redirect(url_for('users.userProfile', userId=userId))

@bp.route('<bookId>/voteBook', methods=["GET","POST"])
def voteForBook(bookId):
    vote = VoteForBook.query.filter_by(userId=current_user.id, bookId = bookId).first()
    if vote:
        return redirect(url_for('books.showBook', bookId=bookId))
    else:
        vote = VoteForBook(
            userId = current_user.id,
            bookId = bookId
        )
    db.session.add(vote)
    db.session.commit()
    return redirect(url_for('books.showBook', bookId=bookId)) 


###commands###
@bp.cli.command("delete-all")
def delete_all():
    User.query.delete()
    db.session.commit()
    logging.info("Deleted all users.")

@bp.cli.command("promote-to-admin")
@click.argument('username')
def promote_to_admin(username):
    user = User.query.filter_by(username=username).first()
    if user:
        if makeUserAdmin(user):
            logging.info(f"User {username} is now admin.")
            db.session.commit()
        else:
            logging.warning(f"User {username} is already admin.")
    else:
        logging.error(f"User {username} doesn't exist.")

@bp.cli.command("info")
@click.argument('username')
def get_user_info(username):
    user = User.query.filter_by(username=username).first()
    if user:
        logging.info(f"<User {username}| is_admin={user.is_admin}>")
    else:
        logging.error(f"User {username} doesn't exist.")

@bp.cli.command("create")
@click.argument('username')
@click.argument('password')
def create_user(username, password):
    user = User.query.filter_by(username=username).first()
    if user:
        logging.error(f"User {username} already exists.")
    else:
        newUser = User(
            username=username,
            profile_desc="",
        )
        newUser.set_password(password)
        db.session.add(newUser)
        db.session.commit()
        logging.info(f"Created user {username}")

