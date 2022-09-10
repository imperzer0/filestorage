# filestorage
Lightweight storage server application

## Installation

### From repository
```bash
git clone https://github.com/imperzer0/privacy-protection-messenger.git
cd filestorage
```

#### Archlinux
```bash
makepkg -sif
```

#### Other distributions
```bash
sudo bash -c ". ./PKGBUILD && notarch_prepare && build && notarch_package"
```

## Usage

### Preparation
Setup <b>mariadb</b> and create user with name filestorage.
Create a password for him (required).
Grant him privileges to create and drop databases.
Grant him ALL PRIVILIGES on a database "filestorage".

### Running
Open up terminal and run commands
```bash
sudo mkdir -p /srv/filestorage/
cd /srv/filestorage/
filestorage --address "http://0.0.0.0:PORT" --password <database user password>
```

### Other
If you wanna drop your database run
```bash
filestorage --destroy-db --password <database user password>
```
