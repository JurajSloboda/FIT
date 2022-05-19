from os import close
from flask import Blueprint, render_template, request, flash, Response, url_for, redirect
from flask_login import login_required
from wtforms.validators import DataRequired

from app.models import Book, Library, LibraryToLibrarian, User, BookCopy,LibrarianOrderBook
from app.forms import AddLibraryForm, LibraryEditForm, SearchLibsByNameForm
from app import db

bp = Blueprint('library', __name__, url_prefix="/library")

@bp.route('/__all')
def allLibraries():
    libraries = Library.query.all()
    return render_template('library/allLibraries.html', title="All Libraries", libraries = libraries)

@bp.route('/addLibrary', methods=["GET","POST"])
@login_required
def addLibrary():
    form = AddLibraryForm()

    if form.validate_on_submit():
        #create group
        library = Library(
            name=form.name.data,
            description=form.description.data,
            address = form.address.data,
            openTime = form.openTime.data,
            closeTime = form.closeTime.data
        )

        db.session.add(library)
        db.session.commit()
        flash("library created!")
        return redirect(url_for('library.showLibrary', libraryId=library.id))
    elif request.method == "GET":
        form.openTime.data = 0
        form.closeTime.data = 24
    return render_template("library/addLibrary.html", title="Add new library", form=form)

@bp.route('<libraryName>/libraryprofile/editLibrary', methods=["GET","POST"])
@login_required
def editLibrary(libraryName):
    form = LibraryEditForm()
    library = Library.query.filter_by(name=libraryName).first_or_404()
    if form.validate_on_submit():
        
        library.name=form.name.data
        library.description=form.description.data
        library.address = form.address.data
        library.openTime = form.openTime.data
        library.closeTime = form.closeTime.data

        db.session().commit()
        flash("Changes saved")

    elif request.method == "GET":
        form.description.data = library.description
        form.name.data = library.name
        form.address.data = library.address
        form.openTime.data = library.openTime
        form.closeTime.data = library.closeTime
        # profileEditForm.profileVisibility.data = current_user.profile_visibility

    return render_template('library/editLibrary.html', title="Library Edit", libraryEditForm=form)

@bp.route('<libraryId>/deleteLibrary')
def deleteLibrary(libraryId):
    books = BookCopy.query.filter_by(libraryId=libraryId)
    librarians = LibraryToLibrarian.query.filter_by(libraryId=libraryId)
    order = LibrarianOrderBook.query.filter_by(libraryId=libraryId)
    library = Library.query.filter_by(id=libraryId)
    library.delete()
    books.delete()
    librarians.delete()
    order.delete()
    db.session.commit()
    return redirect(url_for('library.allLibraries'))

@bp.route('/<libraryId>/libraryprofile')
def showLibrary(libraryId):
    #get the group if it exists
    library = Library.query.filter_by(id=libraryId).first_or_404()

    return render_template("library/showLibrary.html", title=library.name, library=library)

@bp.route('/search', methods=["GET", "POST"])
def searchLibraries():
    form = SearchLibsByNameForm()
    libraries = set()
    if form.validate_on_submit():
        library = Library.query.filter_by(name=form.searchBar.data).first()
        if library:
            libraries.add(library)
        
    else:
        books = Library.query.all()

    return render_template("library/searchLibraries.html", libraries=libraries, form=form)

@bp.route('<libraryId>/allLibrarians', methods=["GET", "POST"])
def allLibrarians(libraryId):
    librariansLib = LibraryToLibrarian.query.filter_by(libraryId=libraryId)
    return render_template("library/allLibrarians.html", librarians=librariansLib)

@bp.route('<libraryId>/inventory', methods=["GET", "POST"])
def inventory(libraryId):
    books = BookCopy.query.filter_by(libraryId=libraryId)
    # inventory = B
    # for book in books:
    #     inventory
    # inventory = Book.query.
    return render_template("library/inventory.html", books=books)
