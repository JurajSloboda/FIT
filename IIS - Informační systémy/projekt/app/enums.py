from enum import IntEnum

class RequestStatus(IntEnum):
    UNPROCESSED = 0
    APPROVED = 1
    DENIED = 2

class UserStatus(IntEnum):
    USER = 0
    LIBRARIAN = 1
    DISTRIBUTOR = 2
    ADMIN = 3

def getListForForm(enumClass):
    return [(int(value), label) for label, value in enumClass.__members__.items()]