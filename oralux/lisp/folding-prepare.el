
(load-library "folding")
(declare (special fold-fold-on-startup
                  fold-keys-already-setup
                  fold-keys-already-setup
                  fold-dont-bind-keys
                  fold-inhibit-startup-message
                  ))
    
(setq fold-fold-on-startup nil)
(setq fold-keys-already-setup nil)
(add-hook 'folding-mode-hook
          (function (lambda()
                      (unless fold-keys-already-setup
                        (setq fold-keys-already-setup t)
                        (define-prefix-command 'ctl-f-folding-mode-prefix)
                        (define-key 'ctl-f-folding-mode-prefix "f" 'fold-fold-region)
                        (define-key  'ctl-f-folding-mode-prefix "e" 'fold-enter)
                        (define-key 'ctl-f-folding-mode-prefix "x" 'fold-exit)
                        (define-key  'ctl-f-folding-mode-prefix "b" 'fold-whole-buffer)
                        (define-key 'ctl-f-folding-mode-prefix "o" 'fold-open-buffer)
                        (define-key 'ctl-f-folding-mode-prefix "h" 'fold-hide)
                        (define-key 'ctl-f-folding-mode-prefix "s" 'fold-show)
                        (define-key 'ctl-f-folding-mode-prefix "t" 'fold-top-level)
                        )
                      (local-set-key "\C-f" 'ctl-f-folding-mode-prefix))))

(setq fold-dont-bind-keys t)
(setq fold-inhibit-startup-message t)

(folding-mode-add-find-file-hook)

;; Comment (Oralux): php-mode
(folding-add-to-marks-list 'php-mode "// {{{" "// }}}")

;; Comment (Oralux): folding-add-to-marks-list instead of fold-add-to-marks-list
(folding-add-to-marks-list 'apache-generic-mode
                        "# {{{" 
                        "# }}}" "")
(folding-add-to-marks-list 'xml-mode
                        "<!-- {" 
                        "<!-- } -->" " --> ")
(folding-add-to-marks-list 'sgml-mode
                        "<!-- {" 
                        "<!-- } -->" " --> ")
(folding-add-to-marks-list 'dtd-mode
                        "<!-- {" 
                        "<!-- } -->" " --> ")
(folding-add-to-marks-list 'c-mode "/* <" "/* > */" "*/")
(folding-add-to-marks-list 'c++-mode
                        "//<" "//>" "")
(folding-add-to-marks-list 'LaTeX-mode "%%% {{{" "%%% }}}" " ")
(folding-add-to-marks-list 'latex2e-mode "%%% {{{" "%%% }}}" " ")
(folding-add-to-marks-list 'latex-mode "%%% {{{" "%%% }}}" " ")
(folding-add-to-marks-list 'BibTeX-mode "%%% {{{" "%%% }}}" " ")
(folding-add-to-marks-list 'lisp-mode ";;; {" ";;; }" "")
(folding-add-to-marks-list 'lex-mode" /* {{{ " " /* }}} */ " "*/")
(folding-add-to-marks-list 'html-helper-mode "<!-- { " "<!-- } -->" "-->")
(folding-add-to-marks-list 'tcl-mode "# {{{" "# }}}" " ")
(folding-add-to-marks-list 'sh-mode "# {{{" "# }}}" " ")
