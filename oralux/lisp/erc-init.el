;;; From erc-init.el --- Various erc hacks that make .emacs too big.
;; Michael Olson http://www.mwolson.org/projects/emacs-config/erc-init.el.html
;;
;; Modified for Oralux: Gilles Casse November 2004
;;  

;;; This should not be evaluated when I use sudo
(when (not (string-equal "root" (getenv "USER")))

(setq-default erc-default-nicks "YourNickname")
(setq my-bitlbee-password "YourPassword")


;; Load erc
(require 'erc)

;;; Variables

;; Set the default value for nickname

(setq my-bitlbee-server "testing.bitlbee.org"
      my-bitlbee-port "6667"
      my-bitlbee-nickname erc-default-nicks
      my-freenode-nickname erc-default-nicks
      my-freenode-password ""
)

;;; Authenticate to NickServ

(defun my-erc-after-connect-stuff (server nick)
  "Authenticate to various NickServ's and the like upon
connecting to the server of their jurisdiction."
  (interactive)
  (cond
   ((string-match "testing\\.bitlbee\\.org" server)
    (erc-message "PRIVMSG"
                 (concat "#bitlbee identify " my-bitlbee-password)))
   ((string-match "freenode\\.net" server)
    (erc-message "PRIVMSG"
                 (concat "NickServ identify " my-freenode-password)))
   ))

(add-hook 'erc-after-connect 'my-erc-after-connect-stuff)

;; Template for connecting to networks
(defun my-erc-register-connection (command server port nick)
  "Create interactive command `command', for connecting to an IRC
server. The command uses interactive mode if passed an argument."
  (fset command
        `(lambda (arg) (interactive "p")
           (if (= 1 arg)
               (erc ,server ,port ,nick erc-user-full-name t)
             (erc-select ,server ,port ,nick)))))

;; Make several functions that start an erc session
(my-erc-register-connection
 'erc-bitlbee "testing.bitlbee.org" "6667" my-bitlbee-nickname)

(my-erc-register-connection
 'erc-freenode "irc.freenode.net" "ircd" my-freenode-nickname)


;; End of check to see if sudo is being used
)

;;; erc-init.el ends here

