import subprocess

Users = [
    #[name, password]
    ["admin", "admin"],
    ["user1", "user"],
    ["user2", "user"],
    ["user3", "user"],
    ["user4", "user"]
]


# #empty database
# subprocess.run(["flask", "users", "delete-all"])

#fill database
for user in Users:
    subprocess.run(["flask", "users", "create"] + user)
