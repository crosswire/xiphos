;; -*-emacs-lisp-*-
;;
;; Emacs startup file for the Debian GNU/Linux gnomeswordLE package
;;
;; Originally contributed by Nils Naumann <naumann@unileoben.ac.at>
;; Modified by Dirk Eddelbuettel <edd@debian.org>
;; Adapted for dh-make by Jim Van Zandt <jrv@vanzandt.mv.com>

;; The gnomeswordLE package follows the Debian/GNU Linux 'emacsen' policy and
;; byte-compiles its elisp files for each 'emacs flavor' (emacs19,
;; xemacs19, emacs20, xemacs20...).  The compiled code is then
;; installed in a subdirectory of the respective site-lisp directory.
;; We have to add this to the load-path:
(setq load-path (nconc load-path (list (concat "/usr/share/"
                                               (symbol-name flavor)
                                               "/site-lisp/gnomeswordLE"))))
