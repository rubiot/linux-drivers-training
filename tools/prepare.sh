#!/bin/bash

USER=$SUDO_USER

MYDIR=$(dirname $0)
LOGDIR=$MYDIR/.prepare
LOGPKGDIR=$LOGDIR/packages
LOGPYPKGDIR=$LOGDIR/pypackages
LOGREPODIR=$LOGDIR/repos

CFGFILE=$MYDIR/prepare.cfg

TFTP_CFG="service tftp
{
    socket_type     = dgram
    protocol        = udp
    wait            = yes
    user            = root
    server          = /usr/sbin/in.tftpd
    server_args     = -s /tftpboot
    disable         = no
}"

log()
{
    echo "`date`: $@"
}

log_error()
{
    log "ERROR: $@"
}

exit_error()
{
    log_error $@
    exit 1
}

run()
{
    $@
}

run_safe()
{
    run $@
    if [ $? != 0 ]; then
        exit_error "Could not execute [$@]"
    fi
}

run_cmd()
{
    if [ ! -e $LOGDIR/$1 ]; then
        $@

        if [ $? == 0 ]; then
            run_safe touch $LOGDIR/$1
        fi
    fi
}

run_try()
{
    TRIES=3
    for i in $(seq $TRIES); do
        $@
        if [ $? == 0 ]; then
            return 0
        fi
    done
    return 1
}

check_user()
{
    if [ $UID != 0 ]; then
        exit_error "Invalid UID. Please run this script as root!"
    fi

    if [ "$USER" == "" ]; then
        exit_error "Invalid USER. Please run this script as root!"
    fi
}

check_logdir()
{
    run_safe mkdir -p $LOGDIR $LOGPKGDIR $LOGPYPKGDIR $LOGREPODIR
    run_safe touch $LOGDIR/checked
}

check_cfg_file()
{
    if [ ! -e $CFGFILE ]; then
        exit_error "Configuration file not found!"
    fi
    source $CFGFILE
}

add_repos_ubuntu()
{
    for repo in `echo $@`; do
        if [ ! -e $LOGREPODIR/$repo ]; then
            log "Adding repo [$repo]..."
            run_safe add-apt-repository -y $repo
            run_safe mkdir -p $LOGREPODIR/$repo
        fi
    done
}

add_extra_repos()
{
    if [ ! -z "$LW_EXTRA_REPOS" ]; then
        add_repos_ubuntu $LW_EXTRA_REPOS
    fi
}

update_aptget()
{
    log "Updating packages..."
    run apt-get update
}

install_packages_ubuntu()
{
    for pkg in `echo $@`; do
        if [ ! -e $LOGPKGDIR/$pkg ]; then
            log "Installing [$pkg]..."
            run_safe run_try apt-get install $pkg --assume-yes
            run_safe touch $LOGPKGDIR/$pkg
        fi
    done
}

install_packages()
{
    update_aptget

    if uname -a|grep -sq 'x86_64'; then
        LW_PACKAGES+=" $LW_PACKAGES_64"
    fi

    run_cmd install_packages_ubuntu $LW_PACKAGES
}

install_python_packages()
{

    if [ "$LW_PYTHON_PACKAGES" == "" ]; then
        return 1;
    fi

    for pkg in `echo $LW_PYTHON_PACKAGES`; do
        if [ ! -e $LOGPYPKGDIR/"$pkg" ]; then
            log "Installing python package [$pkg]..."
            run_safe pip install "$pkg"
            run_safe touch $LOGPYPKGDIR/"$pkg"
        fi
    done
}

extract_files()
{
    if [ "$LW_EXTRACT_FILES" == "" ]; then
        return 1;
    fi

    for field in $LW_EXTRACT_FILES; do

        orig=$(echo "$field" | cut -s -d ":" -f 1)
        dest=$(echo "$field" | cut -s -d ":" -f 2)
        if [ -z "$orig" -o -z "$dest" ]; then
            continue;
        fi

        log "Extracting file [$orig] to [$dest]..."

        run_safe mkdir -p $dest
        run_safe cd $dest
        run_safe tar xfv $orig

	dir=$(echo $(basename ${orig%%.*}))
	if [ ! -z "$dir" -a -d "$dir" ]; then
            run_safe chown $USER:$USER -R $dir
        fi

        run_safe cd -

    done
}

set_path()
{
    if [ -z "$LW_SET_PATH" ]; then
        return 1;
    fi

    BASHRC="/home/$USER/.bashrc"

    for p in "$LW_SET_PATH"; do
        echo "Adding [$p] to the user PATH environment..."
        grep -q "$p" $BASHRC
	if [ $? != 0 ]; then
            echo export PATH=$p:'$PATH' >> $BASHRC
        fi
    done
}

setup_android()
{
    if [ "$LW_ANDROID" != "y" ]; then
        return 1
    fi

    if [ ! -e /usr/lib/i386-linux-gnu/libGL.so ]; then
        log "Creating libGL link..."
        run_safe ln -s /usr/lib/i386-linux-gnu/mesa/libGL.so.1 /usr/lib/i386-linux-gnu/libGL.so
    fi
    if [ ! -e /usr/lib/libpython2.6.so.1.0 ]; then
        log "Creating libpython link..."
        run_safe ln -s /usr/lib/libpython2.7.so.1.0 /usr/lib/libpython2.6.so.1.0
    fi

    UDEV_RULES_FILE="51-android_labworks.rules"
    if [ ! -e /etc/udev/rules.d/$UDEV_RULES_FILE ]; then
        log "Creating Android udev rules file..."
        run_safe sed -i "s/username/$USER/g" $MYDIR/$UDEV_RULES_FILE
        run_safe cp -av $MYDIR/$UDEV_RULES_FILE /etc/udev/rules.d/
    fi
    run_safe service udev restart
}

setup_tftp_server()
{
    if [ "$LW_TFTP_SERVER" != "y" ]; then
        return 1
    fi

    log "Setting up TFTP server..."
    echo "$TFTP_CFG" > /etc/xinetd.d/tftp
    if [ ! -e /etc/xinetd.d/tftp ]; then
        exit_error "Error creating tftp configuration file!"
    fi
    run_safe mkdir -p /tftpboot
    run_safe chmod 777 /tftpboot
    run_safe /etc/init.d/xinetd restart
}

setup_http_server()
{
    if [ "$LW_HTTP_SERVER" != "y" ]; then
        return 1
    fi

    log "Setting up HTTP server..."
    run_safe service lighttpd restart
}

setup_rootfs_nfs()
{
    if [ -z "$LW_ROOTFS_NFS" ]; then
            return 1
    fi

    log "Setting up rootfs via NFS..."

    run_safe mkdir -p $LW_ROOTFS_NFS
    run_safe chown $USER:$USER -R $LW_ROOTFS_NFS

    grep -q "$LW_ROOTFS_NFS 192.168.0.2" /etc/exports
    if [ $? != 0 ]; then
        echo "$LW_ROOTFS_NFS 192.168.0.2(rw,no_root_squash,no_subtree_check)" >> /etc/exports
    fi

    run_safe service nfs-kernel-server restart
}

remove_ssh_know_hosts()
{
    log "Removing SSH known hosts..."
    rm -Rf /home/$USER/.ssh/known_hosts
}

add_user_to_dialout()
{
    log "Adding user to dialout group..."
    run_safe usermod -a -G dialout $USER
}

setup_picocom()
{
    log "Setting up picocom..."
    grep -q "alias picocom" /home/$USER/.bashrc
    if [ $? != 0 ]; then
        echo "alias picocom='picocom /dev/ttyUSB0 -l -b 115200 -f n'" >> ~/.bashrc
    fi
}

setup_terminfo()
{
    if [ "$LW_TERMINFO" != "y" ]; then
            return 1
    fi

    log "Setting up terminfo..."
    grep -q "export TERMINFO" /home/$USER/.bashrc
    if [ $? != 0 ]; then
        echo "export TERMINFO=/lib/terminfo" >> ~/.bashrc
    fi
}

main()
{
    check_user
    check_logdir
    check_cfg_file
    add_extra_repos
    install_packages
    install_python_packages
    run_cmd extract_files
    run_cmd set_path
    run_cmd setup_android
    run_cmd setup_tftp_server
    run_cmd setup_http_server
    run_cmd setup_rootfs_nfs
    run_cmd remove_ssh_know_hosts
    run_cmd add_user_to_dialout
    run_cmd setup_picocom
    run_cmd setup_terminfo

    log "Environment successfully configured! Please logoff and login again to complete installation."
}

main
