# Branching Strategy

**Version**: 1.0
**Last Updated**: 2026-02-14

---

## Overview

wordNebula uses a **modified GitHub Flow** with a development branch for work-in-progress and a protected main branch for production-ready code.

This strategy balances simplicity (solo developer) with professional practices (portfolio project).

---

## Branch Structure

```
main          → Stable, portfolio-ready code (protected)
  ↑            Only merge when features are complete and CI passes
  |
develop       → Active development, integration branch
  ↑            Daily work happens here, can be temporarily broken
  |
feature/*     → Feature branches for experimental/major work
```

---

## Branch Purposes

### `main` - Production Branch (Protected)
- **Purpose**: Stable, portfolio-ready code
- **Protection**: Requires PR + CI passing before merge
- **Tags**: Version tags (v0.1.0, v0.2.0, etc.)
- **When to merge to**: When feature is complete, tested, and ready to showcase

### `develop` - Integration Branch
- **Purpose**: Daily development work
- **Protection**: None (can push directly)
- **When to use**: Most commits go here
- **State**: Can be broken temporarily during development

### `feature/*` - Feature Branches
- **Purpose**: Experimental work, major features, redesigns
- **Naming**: `feature/gap-buffer`, `feature/markdown-export`, etc.
- **Merge to**: `develop` (via PR optional but recommended)
- **Delete**: After merging

---

## Workflows

### Day-to-Day Development

```bash
# Work on develop branch
git checkout develop
git pull origin develop

# Make changes
git add .
git commit -m "Add cursor movement logic"
git push origin develop
```

**CI runs automatically on every push to `develop`** - catches issues early!

### New Feature Development

```bash
# Create feature branch from develop
git checkout develop
git pull origin develop
git checkout -b feature/my-feature

# Work on feature
git add .
git commit -m "Implement my feature"
git push origin feature/my-feature

# Create PR: feature/my-feature → develop
# Review, CI passes, merge
```

### Preparing for Production/Portfolio Review

```bash
# Ensure develop is stable
git checkout develop
cmake --build build
ctest --test-dir build --output-on-failure

# Create PR: develop → main
# GitHub Actions CI must pass
# Merge to main (protected - requires PR)

# Tag the release
git checkout main
git pull origin main
git tag -a v0.1.0 -m "First portfolio release"
git push origin v0.1.0
```

---

## CI Strategy

**Configured in** [.github/workflows/ci.yml](../.github/workflows/ci.yml)

### CI Runs On:
- **Push** to: `main`, `develop`
- **Pull Requests** to: `main`, `develop`

### CI Jobs:
1. **Build and Test** - Compiles project, runs all tests
2. **Static Analysis** - clang-tidy, cppcheck
3. **Format Check** - Verifies code formatting
4. **Sanitizers** - AddressSanitizer, UBSan

### CI Requirements:
- ✅ All jobs must pass before merging to `main`
- ⚠️ `develop` can have failures (work in progress)

---

## Branch Protection Rules

**Applied to**: `main` branch

### Required Settings (via GitHub Web UI):
1. **Require pull request before merging**
   - Require approvals: 0 (solo developer)
   - Dismiss stale reviews: Enabled

2. **Require status checks to pass**
   - Require branches to be up to date: Enabled
   - Required checks:
     - `Build and Test`
     - `Static Analysis`
     - `Code Formatting Check`
     - `Sanitizers`

3. **Do not allow bypassing the above settings**
   - Even admins must follow rules (demonstrates discipline)

4. **Allow force pushes**: Disabled
5. **Allow deletions**: Disabled

---

## How to Set Up Branch Protection (GitHub)

### Step 1: Navigate to Settings
1. Go to your GitHub repository: `https://github.com/YOUR-USERNAME/wordNebula`
2. Click **Settings** tab
3. Click **Branches** in left sidebar

### Step 2: Add Rule for `main`
1. Click **Add branch protection rule**
2. Branch name pattern: `main`

### Step 3: Configure Protection
Enable these options:

- [x] **Require a pull request before merging**
  - Required approvals: 0 (you're solo, but forces PR workflow)
  - Dismiss stale pull request approvals when new commits are pushed

- [x] **Require status checks to pass before merging**
  - [x] Require branches to be up to date before merging
  - Search and add required checks (after first CI run):
    - `Build and Test`
    - `Static Analysis`
    - `Code Formatting Check`
    - `Sanitizers`

- [x] **Require conversation resolution before merging** (optional)

- [x] **Do not allow bypassing the above settings**
  - Forces you to follow your own rules (professional practice)

- [x] **Restrict who can push to matching branches** (optional)
  - Leave empty if solo developer

### Step 4: Save
Click **Create** or **Save changes**

---

## Example: Merging to Main

### Before Branch Protection
```bash
# ❌ Old way (can push directly to main)
git checkout main
git merge develop
git push origin main  # Allowed, but unprofessional
```

### After Branch Protection
```bash
# ✅ Professional way (must use PR)
git checkout develop
git pull origin develop

# Push develop to remote
git push origin develop

# On GitHub:
# 1. Create PR: develop → main
# 2. Wait for CI to pass (all 4 jobs green)
# 3. Review the changes
# 4. Click "Merge pull request"
# 5. Optionally tag: git tag v0.2.0

# You cannot push directly to main anymore!
```

**What happens if you try to push to main?**
```bash
git checkout main
git push origin main
# remote: error: GH006: Protected branch update failed
# GitHub will reject the push!
```

---

## Benefits for Portfolio/Resume

When asked in interviews:

> **"How do you ensure code quality in your projects?"**

You can point to:
- ✅ Branch protection on production branch
- ✅ CI/CD pipeline (GitHub Actions)
- ✅ Required status checks before merge
- ✅ Code formatting automation
- ✅ Static analysis (clang-tidy, cppcheck)
- ✅ Sanitizer builds catching memory errors
- ✅ Test coverage reporting

This demonstrates **professional software engineering practices** beyond "just writing code."

---

## Common Commands Reference

```bash
# Check which branch you're on
git branch

# Switch to develop
git checkout develop

# Create new feature branch
git checkout -b feature/my-feature

# Push and set upstream
git push -u origin feature/my-feature

# Delete local branch (after merge)
git branch -d feature/my-feature

# Delete remote branch (after merge)
git push origin --delete feature/my-feature

# View all branches (local + remote)
git branch -a

# Sync local main with remote
git checkout main
git pull origin main
```

---

## Troubleshooting

### "Can't push to main" error
**Good!** Branch protection is working. Use a PR instead.

### CI failing on develop
**OK during development.** Fix before creating PR to main.

### CI failing on PR to main
**Must fix before merge.** Main branch stays stable.

### Need to bypass protection (emergency)
1. Go to Settings → Branches → Edit rule
2. Temporarily disable "Do not allow bypassing"
3. Make emergency fix
4. Re-enable immediately after

---

## Future Enhancements

As the project grows, consider:
- **Required reviewers** (if collaborators join)
- **CODEOWNERS** file for automatic review requests
- **Release branches** for version maintenance
- **Changelog automation** (conventional commits)
- **Automated semantic versioning**

---

**Remember**: The goal is to demonstrate professional practices, not to overcomplicate. This strategy keeps it simple while showing you understand industry-standard workflows.

---

*For questions about this strategy, see [.github/workflows/ci.yml](../.github/workflows/ci.yml) and [CLAUDE.md](../CLAUDE.md)*
