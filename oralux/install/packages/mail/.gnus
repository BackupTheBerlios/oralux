;; Update strings such as "my-isp.net" to yours.

(setq gnus-nntp-server "news.my-isp.net")
(setq gnus-secondary-select-methods
      '((nnml "private")))

(setq mail-sources
      '((pop :server "mail.my-isp.net"
	     :user "username"
	     :port "pop3"
	     :password "secret")))

;; ;;A non-emacspeak emacs should load .gnus automatically, otherwise the
;; ;;above code should probably be in .emacs.

;; ;; In addition, sending email requires smtpmail setup by this code:
;; ;; lines copied from smtpmail.el 
;; ;;
;; (setq send-mail-function 'smtpmail-send-it)
;; (setq smtpmail-default-smtp-server "mail.my-isp.net")
;; (setq smtpmail-smtp-service "smtp")
;; (setq smtpmail-local-domain "mail.my-isp.net")
;; (setq smtpmail-debug-info t)
;; (load-library "smtpmail")
;; (setq smtpmail-code-conv-from nil)
;; (setq user-full-name "My user name")
;; (setq user-mail-address "My email")

(cond ((string-match "ru" (getenv "LANGUAGE"))
       (print "Russian environment")
       ;; Begin Gnus Russian stuff
       
       ;; Charset adjusting
       (put-charset-property 'cyrillic-iso8859-5 'preferred-coding-system 'koi8-r)
       (define-coding-system-alias 'koi8-u 'koi8-r)
       (setq gnus-group-charset-alist '((".*" koi8-r)))
       (setq gnus-group-posting-charset-alist '((t nil (koi8-r))))
       
       (codepage-setup 1251)
       (define-coding-system-alias 'windows-1251 'cp1251)
       
       ;; End of Gnus Russian stuff.
       )
      )
