SECURITY CONSIDERATIONS FOR ORALUX

1. The configuration files and knoppix home directory could be stored on external devices (hard disk partition, usb key,...). These data will be automatically searched after each boot.
  
2. There are no default passwords.

3. Therefore, all local interactive processes are started by init
    without authorization. Programs that only work for root are
    started using sudo without password. This has the advantage of
    making faults caused by 
    defective software very unlikely, but does not enhance local
    security, since it is fairly easy to switch between the "knoppix"
    and "root" account. The knoppix user should never be allowed for
    external logins (in the case that sshd or similar servers are being launched).
