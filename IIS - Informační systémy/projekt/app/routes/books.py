import click
import logging
from flask import Blueprint, render_template, request, flash, Response, url_for, redirect
from flask_login import login_required, current_user

from app.models import Book, Tag, Order, User, Library, BookCopy
from app.forms import AddBookForm, SearchBooksByTagsOrNameForm, reservationForBook, BookEditForm, OrderForm, LibrarianOrderBook
from app import db

bp = Blueprint('books', __name__, url_prefix="/book")

@bp.route('/__all')
def allBooks():
    books = Book.query.all()
    return render_template('books/allBooks.html', title="All Books", books = books)

@bp.route('/addBook', methods=["GET","POST"])
@login_required
def addBook():
    form = AddBookForm()

    if form.validate_on_submit():
        #create group
        book = Book(
            name=form.name.data,
            description=form.description.data,
            authors = form.authors.data,
            isbn = form.isbn.data,
            year = form.year.data,
            rating = form.rating.data,
            publisher = form.publisher.data
        )

        # tagTokens = form.tags.data.split(sep=",")
        # for tagToken in tagTokens:
        #     if not tagToken: #ignore empty strings
        #         continue
        #     book.addTag(tagToken.strip())
        
        db.session.add(book)
        db.session.commit()
        flash("book created!") #prob won't be seen
        return redirect(url_for('books.showBook', bookId=book.id))
    elif request.method == "GET":
        form.year.data = 2021
        form.rating.data = 0
    return render_template("books/addBook.html", title="Add new book", form=form)

@bp.route('/<bookId>', methods=["GET","POST"])
def showBook(bookId):
    #get the book if it exists
    
    book = Book.query.filter_by(id=bookId).first_or_404()
    orderedby = book.isOrderedBy(book.id)

    form = reservationForBook()
    orderForm = OrderForm()
    if form.validate_on_submit():
        if current_user.is_authenticated:
            order = Order.query.filter_by(orderUserId=current_user.id, orderBookId = bookId )
            order.delete()
            order = Order(
                orderUserId = current_user.id,
                orderBookId = book.id,
                status = 1
            )
            db.session.add(order)
            db.session.commit()
        else:
            return redirect(url_for('auth.register'))
    
    if orderForm.validate_on_submit():
        orderLib = LibrarianOrderBook(
            bookId = bookId,
            libraryId = current_user.getIdOfWorkingLibrary(),
            amount = orderForm.amount.data
        )
        db.session.add(orderLib)
        db.session.commit()
    
    return render_template("books/showBook.html", title=book.name, book=book, form = form, orderForm = orderForm, orderedby = orderedby)

@bp.route('<bookId>/deleteBook')
def deleteBook(bookId):
    book = Book.query.filter_by(id=bookId).first()
    books = BookCopy.query.filter_by(bookId=book.id)
    orders = LibrarianOrderBook.query.filter_by(bookId=book.id)
    order = Order.query.filter_by(orderBookId=book.id)
    book = Book.query.filter_by(id=bookId)
    orders.delete()
    order.delete()
    book.delete()
    books.delete()
    db.session.commit()
    return redirect(url_for('index'))

@bp.route('<bookId>/bookprofile/editBook', methods=["GET","POST"])
@login_required
def editBook(bookId):
    form = BookEditForm()
    book = Book.query.filter_by(id=bookId).first_or_404()
    if form.validate_on_submit():
        
        book.name=form.name.data
        book.description=form.description.data
        book.authors = form.authors.data
        book.isbn = form.isbn.data
        book.year = form.year.data
        book.rating = form.rating.data
        book.publisher = form.publisher.data
        # library.openTime = form.openTime.data,
        # library.closeTime = form.closeTime.data

        db.session().commit()
        flash("Changes saved")

    elif request.method == "GET":
        form.description.data = book.description
        form.name.data = book.name
        form.authors.data = book.authors
        form.isbn.data = book.isbn
        form.year.data = book.year
        form.rating.data = book.rating
        form.publisher.data = book.publisher
        # profileEditForm.profileVisibility.data = current_user.profile_visibility

    return render_template('books/editBook.html', title="Book Edit", bookEditForm=form)

@bp.route('/search', methods=["GET", "POST"])
def searchBooks():
    form = SearchBooksByTagsOrNameForm()

    books = set()
    if form.validate_on_submit():
        keywords = form.searchBar.data.split(",")
        for keyword in map(str.strip,keywords):

            namedBook = Book.query.filter_by(name=keyword).first()
            if namedBook:
                books.add(namedBook)

            #add all groups tagged keyword
            tag = Tag.query.filter_by(keyword=keyword).first()
            if tag:
                for taggedBook in tag.tagged_books:
                    books.add(taggedBook)
        
    else:
        books = Book.query.all()

    return render_template("books/searchBooks.html", books=books, form=form)

@bp.route('/<libraryId>/<bookId>/addToLibrary', methods=["GET", "POST"])
def addToMyLibrary(libraryId,bookId):
    booktolib = BookCopy(
        bookId=bookId,
        libraryId=libraryId
    )
    db.session.add(booktolib)
    db.session.commit()
    return redirect(url_for('books.showBook', bookId=bookId))

@bp.route('/<userId>/myBooks', methods=["GET", "POST"])
def myBooks(userId):
    books = Order.query.filter_by(orderUserId=userId)
    return render_template("books/myBooks.html", books=books)