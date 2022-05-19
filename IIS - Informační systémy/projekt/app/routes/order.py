import click
import logging
from flask import Blueprint, render_template, request, flash, Response, url_for, redirect
from flask_login import login_required, current_user

from app.models import Order, BookCopy, Library, Book, User,LibrarianOrderBook #,  LibrarianOrder, LibrarianOrderBook
# from app.forms import ProfileEditForm, PasswordChangeForm
from app import db

bp = Blueprint('order', __name__, url_prefix="/order")

@bp.route('/__all')
def allOrders():
    orders = Order.query.filter_by(status=1)
    return render_template('orders/allOrders.html', title="All Orders", orders = orders)

@bp.route('/<orderId>', methods=["GET"])
def showOrder(orderId):
    #get the book if it exists
    
    order =  Order.query.filter_by(id=orderId).first_or_404()

    user = order.getUserofOrder(order.orderUserId)
    book = Book.query.filter_by(id=order.orderBookId).first()
    return render_template("orders/showOrder.html", title="order", user = user, book=book)

@bp.route('/<bookId>/borrow', methods=["GET","POST"])
def borrowBook(bookId):
    order =  Order.query.filter_by(orderBookId=bookId).first()
    # if order.isBorrowed():
    #     return redirect(url_for('books.showBook', bookId=bookId))
    order.status = 2
    user = User.query.filter_by(id=current_user.id).first()
    books = BookCopy.query.filter_by(bookId=bookId, libraryId=user.getIdOfWorkingLibrary())
    for book in books:
        if book.currentStatus == 0:
            book.currentStatus = 1
            return redirect(url_for('books.showBook', bookId=bookId))
    
    return redirect(url_for('index'))

@bp.route('/showLibraryOrders')
def showLibraryOrders():
    orders = LibrarianOrderBook.query.all()
    return render_template('orders/showLibraryOrders.html', title="All Library Orders", orders = orders)

@bp.route('/<orderId>/showLibraryOrder')
def showLibraryOrder(orderId):
    order = LibrarianOrderBook.query.filter_by(id=orderId).first()
    return render_template('orders/showLibraryOrder.html', title="All Library Orders", order = order)

@bp.route('/<orderId>/showLibraryOrder/complete')
def completeOrder(orderId):
    order = LibrarianOrderBook.query.filter_by(id=orderId).first()
    for i in range(order.amount):
        book = BookCopy(
            bookId = order.bookId,
            libraryId = order.libraryId
        )
        db.session.add(book)
        db.session.commit()
    order = LibrarianOrderBook.query.filter_by(id=orderId)
    order.delete()
    db.session.commit()
    return redirect(url_for('order.showLibraryOrders'))

# @bp.route('/<libraryId>/<librarianId>/Cart', methods=["GET","POST"])
# def Cart(libraryId,librarianId):
#     #todooo
#     return render_template("orders/Cart.html", title="cart")

# @bp.route('/<libraryId>/<librarianId>/CartSend', methods=["GET","POST"])
# def SendOrderToDistributors(libraryId,librarianId):
#     # tododoo
#     return redirect(url_for('index'))

# @bp.route('/<orderId>/showAllLibraryOrders', methods=["GET"])
# def showAllLibraryOrders(orderId):
#     #todoooo
#     orders =  LibrarianOrder.query.all()
#     return render_template("orders/showAllLibraryOrders.html", orders = orders)

# @bp.route('/<libraryId>/showAllLibraryOrders/showOrder', methods=["GET"])
# def showLibraryOrder(libraryId):
#     #todooo
#     orders =  LibrarianOrderBook.query.filter_by(id=libraryId)
#     return render_template("orders/showLibraryOrder.html", orderedbook = orders)

# @bp.route('/<libraryId>/<orderId>/showAllLibraryOrders/showOrder/completeorder', methods=["GET"])
# def showLibraryOrderComplete(libraryId,orderId):
#     #todooo
#     #delete all from orders and add it to library inventory
#     return redirect(url_for('orders/showAllLibraryOrders.html'))

@bp.route('/reservations', methods=["GET", "POST"])
def reservations():
    library = Library.query.filter_by(id=current_user.getIdOfWorkingLibrary()).first()
    books = BookCopy.query.filter_by(libraryId=library.id).all()
    orders = Order.query.limit(0).all()
    for book in books:
        orders.append(Order.query.filter_by(orderBookId=book.id).first())

    return render_template('orders/reservations.html', title="All Orders", orders = orders)
