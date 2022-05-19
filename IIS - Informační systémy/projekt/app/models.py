from enum import unique
from app import db, login
from flask_login import UserMixin, AnonymousUserMixin
from werkzeug.security import generate_password_hash, check_password_hash
from datetime import datetime
import logging
from app.enums import UserStatus

# book_tag_assoc = db.Table('book_tag_assoc',
#     db.Column('book_id', db.Integer, db.ForeignKey('booktitle.id'), primary_key=True),
#     db.Column('tag_id', db.Integer, db.ForeignKey('tag.id'), primary_key=True)
# )

# order_book_assoc = db.Table('order_book_assoc',
#     db.Column('book_id', db.Integer, db.ForeignKey('book.id'), primary_key=True),
#     db.Column('order_id', db.Integer, db.ForeignKey('order.id'), primary_key=True)
# )

# order_user_assoc = db.Table('order_user_assoc',
#     db.Column('user_id', db.Integer, db.ForeignKey('user.id'), primary_key=True),
#     db.Column('order_id', db.Integer, db.ForeignKey('order.id'), primary_key=True)
# )

# book_library_assoc = db.Table('book_library_assoc', 
#     db.Column('book_id', db.Integer, db.ForeignKey('book.id'), primary_key=True),
#     db.Column('library_id', db.Integer, db.ForeignKey('library.id'), primary_key=True)
# )

# librarian_library_assoc = db.Table('librarian_library_assoc',
#     db.Column('user_id', db.Integer, db.ForeignKey('user.id'), primary_key=True),
#     db.Column('library_id', db.Integer, db.ForeignKey('library.id'), primary_key=True)
# )

class AnonymousUser(AnonymousUserMixin):
    def isAdmin(self):
        return False

class User(UserMixin, db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(80), index=True, unique=True)
    password_hash = db.Column(db.String(128))

    profile_desc = db.Column(db.Text)
    userStatus = db.Column(db.Integer, nullable=False, default=UserStatus.USER)

    # may be deleted probably
    is_admin = db.Column(db.Boolean, default=False)
    is_librarian = db.Column(db.Boolean, default=False)
    is_distributor = db.Column(db.Boolean, default=False)

    library_id = db.Column(db.Integer, db.ForeignKey('user.id'))

    # orders = db.relationship(
    #     'Order',
    #     secondary=order_user_assoc,
    #     backref=db.backref('ordered_user', lazy='dynamic'),
    #     lazy='dynamic'
    # )

    def addOrder(self, order):
        self.orders.append(order)
        return True
    
    def __repr__(self):
        return f"<User {self.username}>"

    #authentification
    def set_password(self, password):
        self.password_hash = generate_password_hash(password)

    def check_password(self, password):
        return check_password_hash(self.password_hash, password)

    # get and set user status
    def getUserStatus(self, user):
        status = user.userStatus
        return \
            (status == UserStatus.USER) or \
            (status == UserStatus.LIBRARIAN) or \
            (status == UserStatus.DISTRIBUTOR) or \
            (status == UserStatus.ADMIN) 

    def setAdmin(self):
        self.userStatus = UserStatus.ADMIN
    def setLibrarian(self):
        self.userStatus = UserStatus.LIBRARIAN
    def setDistributor(self):
        self.userStatus = UserStatus.DISTRIBUTOR
    def setUser(self):
        self.userStatus = UserStatus.USER

    def isAdmin(self):
        if self.userStatus == UserStatus.ADMIN :
            return True
        else:
            return False

    def isLibrarian(self):
        if self.userStatus == UserStatus.LIBRARIAN or self.userStatus == UserStatus.ADMIN:
            return True
        else:
            return False
    
    def isDistributor(self):
        if self.userStatus == UserStatus.DISTRIBUTOR or self.userStatus == UserStatus.ADMIN:
            return True
        else:
            return False

    def getLibraryForLibrarian(self):
        librariansLib = LibraryToLibrarian.query.filter_by(librarianId=self.id).first()
        library = Library.query.filter_by(id=librariansLib.libraryId).first()
        return library
        # library = Library.query.filter_by(id=self.librarian_of).first()
        # if library:
        #     return library.name
        # return ""

    def getIdOfWorkingLibrary(self):
        librariansLib = LibraryToLibrarian.query.filter_by(librarianId=self.id).first()
        return librariansLib.libraryId

    def getLibraryName(self):
        if self.hasLibrary():
            library = self.getLibraryForLibrarian()
            if library:
                return library.name
        return ""

    def hasVoted(self,bookId):
        votes = VoteForBook.query.filter_by(userId=self.id).all()
        for vote in votes:
            if vote.bookId == bookId:
                return True
        return False
    
    def hasLibrary(self):
        library = LibraryToLibrarian.query.filter_by(librarianId=self.id).first()
        if library:
            return True
        return False


# using default is_authentificated
@login.user_loader
def load_user(id):
    return User.query.get(int(id))

def makeUserAdmin(user:User):
    if not user.is_admin:
        user.is_admin=True
        return True
    else:
        return False

class BookCopy(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    bookId = db.Column(db.Integer, db.ForeignKey('book.id'))
    libraryId = db.Column(db.Integer, db.ForeignKey('library.id'))
    status = db.Column(db.String(80), default = "FREE")
    currentStatus = db.Column(db.Integer, default = 0)

    def getNameOfBook(self):
            book = Book.query.filter_by(id=self.bookId).first() 
            return book.name
    
###title of book
class Book(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(160), unique=True)
    description = db.Column(db.Text)

    year = db.Column(db.Integer)
    isbn = db.Column(db.String(160))
    publisher = db.Column(db.String(160))
    rating =  db.Column(db.Integer, default = 0)
    authors = db.Column(db.Text) 
    # tags = db.relationship(
    #     'Tag',
    #     secondary=book_tag_assoc,
    #     backref=db.backref('tagged_books', lazy='dynamic'),
    #     lazy='dynamic'
    # )

    # orders = db.relationship(
    #     'Order',
    #     secondary=order_book_assoc,
    #     backref=db.backref('ordered_books', lazy='dynamic'),
    #     lazy='dynamic'
    # )

    # def addOrder(self, order):
    #     self.orders.append(order)
    #     return True

    def isOrderedBy(self, bookId):
        orders = Order.query.filter_by(orderBookId = bookId)
        return orders

    def getBookForLibrary(self):
        bookLib = BookCopy.query.filter_by(bookId=self.id).first()
        library = Library.query.filter_by(id=bookLib.libraryId).first()
        return library

    def getLibraryName(self):
        if self.hasLibrary():
            library = self.getBookForLibrary()
            if library:
                return library.name
        return ""
        # libraries = Library.query
        # for library in libraries:
        #     librarian = library.librarians.filter_by(id=self.id).first()
        #     if librarian:
        #         return library.name
    def hasLibrary(self):
        library = BookCopy.query.filter_by(bookId=self.id).first()
        if library:
            return True
        return False
    
    def isAvailable(self, userId):
        user = User.query.filter_by(id = userId).first()
        bookcopy = BookCopy.query.filter_by(libraryId = user.getIdOfWorkingLibrary(), bookId = self.id)
        for book in bookcopy:
            if book.status == "FREE":
                return True
        return False
    
    def getVotes(self):
        votes = VoteForBook.query.filter_by(bookId = self.id)
        i = 0
        for vote in votes:
            i = i + 1
        return i
    # def addTag(self, tagStr):
    #     # does the tag exist?
    #     tag = Tag.query.filter_by(keyword=tagStr).first()
    #     if not tag:
    #         logging.debug(f"Creating Tag {tagStr}.")
    #         #create tag
    #         tag = Tag(keyword=tagStr)
    #         db.session.add(tag)
    #         #add tag to group
    #         logging.debug(f"Adding tag {tagStr}.")
    #         self.tags.append(tag)
    #         return True
    #     # else:
    #         #check if this group is already tagged
    #         # if not self.tags.filter_by(id=tag.id).first():
    #         #     logging.debug(f"Adding tag {tagStr}.")
    #         #     #add tag to group
    #         #     self.tags.append(tag)
    #         #     return True
    #         # else:
    #         #     logging.debug(f"Book already has tag {tagStr}.")
    #         #     return False

    # def removeTag(self, tagStr):
    #         # does the tag exist?
    #         tag = Tag.query.filter_by(keyword=tagStr).first()
    #         if not tag:
    #             #no tag to remove
    #             logging.debug(f"Tag {tagStr} you are trying to remove doesn't exits.")
    #             return True
    #         else:
    #         #check if this group is already tagged
    #             if self.tags.filter_by(id=tag.id).first():
    #                 #remove tag from group
    #                 logging.debug(f"Removing tag {tagStr}.")
    #                 self.tags.remove(tag)
    #                 return True
    #             else:
    #                 logging.debug(f"This book doesn't have tag {tagStr}.")
    #                 return False

class Library(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(160))
    description = db.Column(db.Text)
    address = db.Column(db.String(160))
    openTime = db.Column(db.Integer, default = 0)
    closeTime = db.Column(db.Integer,  default = 24)

    # inventoryBooks = db.relationship(
    #     'Book',
    #     secondary=book_library_assoc,
    #     backref=db.backref('inventoryBooks', lazy='dynamic'),
    #     lazy='dynamic'
    # )

    # librarians = db.relationship(
    #     'User',
    #     secondary=librarian_library_assoc,
    #     backref=db.backref('librarians', lazy='dynamic'),
    #     lazy='dynamic'
    # )

    # def addLibrarian(self, username):
    #     user = User.query.filter_by(username=username).first()
    #     self.librarians.append(user)
    #     return True

    # def removeLibrarian(self,librarianId):
    #     user = User.query.filter_by(id=librarianId).first()
    #     self.librarians.remove(user)
    #     return True
    
    def addBook(self, bookId):
        # does the tag exist?
        book = Tag.query.filter_by(keyword=bookId).first()
        if book.isNotInLibrary():
            self.inventory_books.append(book)
            return True
        else:
            return False

    def getInventory(self):
        return self.inventoryBooks

class Order(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    orderUserId = db.Column(db.Integer,  db.ForeignKey('user.id') )
    orderBookId = db.Column(db.Integer, db.ForeignKey('book.id'))
    status = db.Column(db.Integer, default = 0)


    def getLibraryId(self):
        user = User.query.filter_by(id=self.orderUserId).first_or_404()
        return user.getIdOfWorkingLibrary()
    def getUserofOrder(self,userId):
        user =  User.query.filter_by(id=userId).first_or_404()
        return user.username
    def getBookofOrder(self):
        book =  Book.query.filter_by(id=self.orderBookId).first_or_404()
        return book.name
    def isBorrowed(self):
        if self.status == "BORROWED":
            return True
        return False

class Tag(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    keyword = db.Column(db.String(80), index=True, unique=True)

##associations
class LibraryToLibrarian(db.Model):
    librarianId = db.Column(db.Integer, db.ForeignKey('user.id'), primary_key=True)
    libraryId = db.Column(db.Integer, db.ForeignKey('library.id'), primary_key=True)

    def getNameOfLibrarian(self):
        librarian = User.query.filter_by(id=self.librarianId).first() 
        return librarian.username

class LibrarianOrderBook(db.Model):
    id = db.Column(db.Integer, primary_key=True) 
    bookId = db.Column(db.Integer, db.ForeignKey('book.id'))
    libraryId = db.Column(db.Integer, db.ForeignKey('library.id'))
    amount = db.Column(db.Integer, default = 1)

    def getUserofOrder(self,userId):
        user =  User.query.filter_by(id=userId).first_or_404()
        return user.username
    def getBookName(self):
        book =  Book.query.filter_by(id=self.bookId).first_or_404()
        return book.name

class VoteForBook(db.Model):
    userId = db.Column(db.Integer, db.ForeignKey('user.id'), primary_key=True)
    bookId = db.Column(db.Integer, db.ForeignKey('book.id'), primary_key=True)
