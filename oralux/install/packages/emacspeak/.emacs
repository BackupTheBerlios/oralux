;; dot emacs
;; 2003, 2004, Oralux Team (contact@oralux.org)

;; w3 
;; set load-path 
;;(setq load-path (cons "/usr/local/src/w3" load-path)) 

;; and loat it 
(require 'w3-auto) 

;; w3m
(setq w3m-key-binding 'info)

;; Babel 
(setq load-path (cons "/usr/share/oralux/install/packages/misc" load-path)) 

;; and load what we need 
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

;; The punctuation is not said (except for Multispeech)
  (cond (
	 (string-match "/usr/local/lib/multispeech/speech_server" (getenv "DTK_PROGRAM"))
	 (dtk-set-punctuations "all"))
	(
	 t 
	 (dtk-set-punctuations "none"))
	)

  ;; The default speech rate is lowered
  (dtk-set-rate 135 t)
  
  ;; Selecting the default language (temporary name) for EFM
  (fst-set-language 
   (cond (
	  (string-match "^fr" (getenv "LANGUAGE"))
	  "french")
	 (t "english")
	 )
   )
)

;; vm
(setq load-path (cons "/usr/share/emacs/site-lisp/vm" load-path))
(require 'vm)

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

(setq load-path (cons "~/.emacs.d" load-path)) 
(load-file "~/.emacs.d/php-mode.el")
;; (folding-add-to-marks-list 'php-mode "// {{{" "// }}}")

(cond (
       (string-match "ru" (getenv "LANGUAGE"))
       (load-file "~/.emacs.d/Russian.el")
       (print "Russian environment")
       )
      (t 
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
 '(tmm-mid-prompt ":"))
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

;; keyboard
(set-language-environment 'latin-1)
(set-terminal-coding-system 'latin-1)
(set-keyboard-coding-system 'latin-1) 

;;erc
(cond
 ((featurep 'emacspeak)
  (require 'erc-speak))
 (t
  (require 'erc)))

;; Unbind C-z to shell instead of suspend-emacs (it is too easy to press C-z and hence suspending Emacspeak).
;; Rather use C-z C-z to suspend emacs
(global-unset-key "\C-z")
(global-unset-key "\C-x\C-z")
(global-set-key "\C-z\C-z" 'suspend-emacs)

;; SES
(add-to-list 'load-path "/usr/share/emacs/site-lisp/ses")
(autoload 'ses-mode "ses.el" "Spreadsheet mode" t)
