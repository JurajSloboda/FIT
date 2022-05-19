from app import create_app

main_app = create_app()

from app import models

if __name__ == "__main__":
    main_app.run()
