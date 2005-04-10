;; dot emacs
;; 2003, 2004, Oralux Team (contact@oralux.org)

; Just for convenience
(require 'dired-x)
(require 'saveplace)
(auto-compression-mode t)
(setq-default transient-mark-mode t)
(setq-default save-place t)
;(setq make-backup-files nil)

; Compilation process
(setq compilation-ask-about-save nil)
(setq compile-command "make -ks ")


(setq load-path (cons "/usr/share/oralux/lisp" load-path)) 

;; w3 
(require 'w3-auto) 

;; w3m
(load-library "emacs-w3m-prepare")

;; Babel 
(autoload 'babel "babel" 
"Use a web translation service to translate the message MSG." t) 
(autoload 'babel-region "babel" 
"Use a web translation service to translate the current region." t) 
(autoload 'babel-as-string "babel" 
"Use a web translation service to translate MSG, returning a string." t) 
(autoload 'babel-buffer "babel" 
"Use a web translation service to translate the current buffer." t) 

;; Automatically load folding mode when folding marks are found
;; (if (load "folding" 'nomessage 'noerror)
;;    (folding-mode-add-find-file-hook))

;; emacspeak
(when (featurep 'emacspeak)
  
  ;; Auditory icons are used in Multispeech
  ;; and the punctuation is not said with other speech servers
  (cond (
	 (string-match "multispeech" (getenv "DTK_PROGRAM"))
	 (setq-default emacspeak-use-auditory-icons t))
	(
	 t 
	 (dtk-set-punctuations "none"))
	)
  
  ;; The default speech rate is lowered
  ;;(dtk-set-rate 135 t)
  
  ;; Selecting the default language (temporary name) for EFM
  (fst-set-language 
   (cond (
	  (string-match "^fr" (getenv "LANGUAGE"))
	  "french")
	 (t "english")
	 )
   )
)


;; Useful for Mail too
(display-time)

;; Mail
;; Update the two following lines:
(setq user-mail-address "username@yourISP")
(setq user-full-name "MyFirstname MyName")

(setq mail-from-style 'angles)
(setq mail-specify-envelope-from t)
(setq mail-archive-file-name "~/mail/outbox")
;; Uncomment the following lisp code to import 
;; the signature file in your mails. 
;; And create you signature file under /home/knoppix 
;;(setq mail-signature t mail-signature-file "~/.signature")

;; The menu uses VM

(define-key menu-bar-tools-menu [rmail] '("Read Mail" . vm))
(define-key-after menu-bar-tools-menu [smail] '("Send Mail" . vm-mail) 'rmail)

;;; Key customizations.
(global-set-key "\C-xg" 'goto-line)
(global-set-key "\C-xy" 'insert-register)
(global-set-key [home] 'beginning-of-line)
(global-set-key [end] 'end-of-line)
(global-set-key [?\e home] 'beginning-of-buffer)
(global-set-key [?\e end] 'end-of-buffer)

(global-set-key [?\e left] 'backward-word)
(global-set-key [?\e right] 'forward-word)

(when (featurep 'emacspeak)
  (defun emacspeak-speak-rest-of-page ()
    (interactive)
    (emacspeak-speak-page '1)
    )
  (global-set-key [?\e up] 'emacspeak-speak-line)
  (global-set-key [?\e down] 'emacspeak-speak-rest-of-page)

  (defun emacspeak-speak-start-of-paragraph ()
    (interactive)
    (emacspeak-speak-paragraph '-1)
    )
  (defun emacspeak-speak-rest-of-paragraph ()
    (interactive)
    (emacspeak-speak-paragraph '1)
    )
  (global-set-key [prior] 'emacspeak-speak-start-of-paragraph)
  (global-set-key [next] 'emacspeak-speak-rest-of-paragraph)
)


(global-set-key [f1] 'help-for-help)
(global-set-key [f2] 'save-buffer)
(global-set-key [f3] 'find-file)
(global-set-key [f4] 'switch-to-buffer)
(global-set-key [f5] 'delete-other-windows)
(global-set-key [f6] 'delete-window)
(global-set-key [f7] 'grep)
(global-set-key [f8] 'kill-buffer)
(global-set-key [f9] 'compile)
(global-set-key [f11] 'bbdb)
(global-set-key [f12] 'other-window)
(global-set-key [f13] 'dtk-set-next-language)
(global-set-key [f15] 'gnus)
(global-set-key [f16] 'shell)
(global-set-key [f17] 'find-name-dired)
(global-set-key [f18] 'w3m)
(global-set-key [f19] 'find-grep-dired)
(global-set-key [f20] 'server-start)


(setq load-path (cons "~/.emacs.d" load-path)) 
;;(require 'php-mode)

(cond (
       (string-match "ru" (getenv "LANGUAGE"))
       (load-file "~/.emacs.d/Russian.el")
       (print "Russian environment")
       )
      (t 
       (set-language-environment 'latin-1)
       (set-terminal-coding-system 'latin-1)
       (set-keyboard-coding-system 'iso-latin-1)
       (setq default-buffer-file-coding-system 'iso-latin-1)
       (print "Latin-1 environment")
       )
      )

(custom-set-variables
  ;; custom-set-variables was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 '(Man-notify-method (quote pushy))
 '(ediff-auto-refine-limit 1400 t)
 '(ediff-diff3-options "")
 '(ediff-forward-word-function (quote ediff-forward-word) t)
 '(ediff-ignore-similar-regions nil t)
 '(ediff-quit-widened t t)
 '(emacspeak-ispell-max-choices 40 t)
 '(ispell-program-name "aspell")
 '(newsticker-html-renderer (quote w3m-region))
 '(newsticker-url-list (quote (("New York Times" "http://www.nytimes.com/services/xml/rss/nyt/HomePage.xml" nil nil nil))))
 '(ps-lpr-command "lp")
 '(tmm-mid-prompt #(":" 0 1 (personality inaudible)))
 '(w3m-after-cursor-move-hook (quote (w3m-highlight-current-anchor w3m-auto-show))))
(custom-set-faces
  ;; custom-set-faces was added by Custom -- don't edit or cut/paste it!
  ;; Your init file should contain only one such instance.
 '(ediff-current-diff-face-A ((((class color)) (:background "pale green" :foreground "white"))))
 '(ediff-current-diff-face-B ((((class color)) (:background "Yellow" :foreground "white"))))
 '(ediff-current-diff-face-C ((((class color)) (:background "Pink" :foreground "white"))))
 '(ediff-even-diff-face-Ancestor ((((class color)) (:background "Grey" :foreground "red"))))
 '(ediff-even-diff-face-B ((((class color)) (:background "Grey" :foreground "red"))))
 '(ediff-odd-diff-face-A ((((class color)) (:background "Grey" :foreground "black"))))
 '(ediff-odd-diff-face-C ((((class color)) (:background "Grey" :foreground "black")))))

;;erc
(cond
 ((featurep 'emacspeak)
  (require 'erc-speak))
 (t
  (require 'erc)))

;; SES
(add-to-list 'load-path "~/SPREADSHEET/ses21-031130/")
(autoload 'ses-mode "ses.el" "Spreadsheet mode" t)

;; Unbind C-z to shell instead of suspend-emacs (it is too easy to press C-z and hence suspending Emacspeak).
;; Rather use C-z C-z to suspend emacs
(global-unset-key "\C-z")
(global-unset-key "\C-x\C-z")
(global-set-key "\C-z\C-z" 'suspend-emacs)

(set-foreground-color "white")
(set-background-color "black")
(global-font-lock-mode t)


;; (cond (
;;        (string-match "^us" (getenv "LANGUAGE"))
;;        (setq ispell-dictionary "english")
;;        )
;;       )

;; (setq-default ispell-program-name "aspell")
;; (autoload 'flyspell-mode "flyspell" "On-the-fly spelling checker." t)
;; (autoload 'flyspell-delay-command "flyspell" "Delay on command." t)
;; (autoload 'tex-mode-flyspell-verify "flyspell" "" t)

;;(load-library "functions-keys")
(load-library "vm-prepare")
(load-library "bbdb-prepare")
(bbdb-insinuate-vm)

(load-library "folding-prepare")

;;(require 'catdoc)   
;;(require 'tramp)
;; (setq backup-by-copying t)
;; (setq backup-directory-alist '(("." . "~/.backups/")))
;; (setq delete-old-versions t)
;; (setq kept-new-versions 6)
;; (setq kept-old-versions 2)

;; bitlbee
(load-library "erc-init")
;;(global-set-key "\C-ceb" 'erc-bitlbee)

;; (setq fill-column '70)
;; (setq text-mode-hook 'turn-on-auto-fill)


;; calendar
(setq european-calendar-style t)
;;appointments
(display-time)
(add-hook 'diary-hook 'appt-make-list)
(if (file-readable-p diary-file)
    (diary 0))

;;newsticker
  (autoload 'w3m-region "w3m"
    "Render region in current buffer and replace with result." t)
;;(add-to-list 'load-path "~/.emacs.d")
(autoload 'newsticker-start "newsticker" "Emacs Newsticker" t)
(autoload 'newsticker-show-news "newsticker" "Emacs Newsticker" t)

;; Newsticker-mode supports imenu. It allows for navigating with the help
;; of a menu. In order to use this feature you should also add the
;; following.

(add-hook 'newsticker-mode-hook 'imenu-add-menubar-index)
(setq browse-url-browser-function 'w3m-browse-url)

(setq smtpmail-default-smtp-server "mySMTPserver")
(setq smtpmail-local-domain nil)
(setq send-mail-function 'smtpmail-send-it)    
(load-library "smtpmail")


;; same window for help buffer,...
(setq same-window-buffer-names
      (append '("*Help*" "*grep*" "*compilation*" "*Occur*")
              same-window-buffer-names))


;;(setq smtpmail-debug-info nil)

;; Gnus
;; (load-library "message")
;; (setq message-send-mail-function 'smtpmail-send-it)

;;(server-start)
