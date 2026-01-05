# Security Considerations & Recommendations

## ⚠️ Current Security Issues

This project is designed for **educational purposes** and demonstrates data structures. It contains several security vulnerabilities that **MUST** be addressed before any production use.

### Critical Issues

#### 1. Plain Text Password Storage 🔴
**Current Implementation**: Passwords are stored in plain text in `user.txt`

**Risk**: If the data file is compromised, all user passwords are exposed.

**Solution**: Implement password hashing
```cpp
// Recommended: Use a proven library
#include <openssl/sha.h>
// or use bcrypt, argon2

std::string hashPassword(const std::string& password, const std::string& salt) {
    // Use bcrypt or argon2 for production
    // Example with SHA-256 (NOT recommended for production):
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, (password + salt).c_str(), password.length() + salt.length());
    SHA256_Final(hash, &sha256);
    
    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Better: Use bcrypt (requires library)
// std::string hashed = bcrypt::generateHash(password);
// bool valid = bcrypt::validatePassword(password, hashed);
```

#### 2. No Input Sanitization 🔴
**Current Implementation**: User inputs are not sanitized

**Risk**: Potential for injection attacks, buffer overflows, or crashes

**Solution**: Add comprehensive input validation
```cpp
std::string sanitizeInput(const std::string& input) {
    std::string clean;
    for (char c : input) {
        // Remove control characters
        if (c >= 32 && c <= 126) {
            clean += c;
        }
    }
    return clean;
}

bool isValidUsername(const std::string& username) {
    // Only alphanumeric and underscores
    for (char c : username) {
        if (!isalnum(c) && c != '_') {
            return false;
        }
    }
    return username.length() >= 3 && username.length() <= 20;
}
```

#### 3. No Session Management 🟡
**Current Implementation**: Single user login per application instance

**Risk**: No protection against session hijacking or concurrent access

**Solution**: Implement session tokens
```cpp
class SessionManager {
private:
    HashMap<std::string, SessionData> activeSessions;
    
public:
    std::string createSession(ull userID) {
        // Generate secure random token
        std::string token = generateSecureToken();
        SessionData session;
        session.userID = userID;
        session.createdAt = getCurrentTimestamp();
        session.expiresAt = session.createdAt + SESSION_DURATION;
        activeSessions[token] = session;
        return token;
    }
    
    bool validateSession(const std::string& token) {
        if (!activeSessions.contains(token)) return false;
        auto session = activeSessions[token];
        return getCurrentTimestamp() < session.expiresAt;
    }
};
```

#### 4. File Permission Issues 🟡
**Current Implementation**: No file permission checks

**Risk**: Data files could be world-readable

**Solution**: Set appropriate file permissions
```cpp
#include <sys/stat.h>

void secureDataFile(const std::string& filename) {
    // Set file permissions to owner read/write only (0600)
    chmod(filename.c_str(), S_IRUSR | S_IWUSR);
}
```

### Medium Priority Issues

#### 5. No Rate Limiting 🟡
**Risk**: Brute force attacks on login

**Solution**: Implement rate limiting
```cpp
class RateLimiter {
private:
    HashMap<std::string, std::vector<time_t>> attempts;
    const int MAX_ATTEMPTS = 5;
    const int WINDOW_SECONDS = 300; // 5 minutes
    
public:
    bool checkLimit(const std::string& identifier) {
        time_t now = time(nullptr);
        auto& hist = attempts[identifier];
        
        // Remove old attempts
        hist.erase(
            std::remove_if(hist.begin(), hist.end(),
                [now, this](time_t t) { return now - t > WINDOW_SECONDS; }),
            hist.end()
        );
        
        if (hist.size() >= MAX_ATTEMPTS) {
            return false; // Rate limit exceeded
        }
        
        hist.push_back(now);
        return true;
    }
};
```

#### 6. No HTTPS/TLS 🟡
**Current State**: CLI application (no network layer)

**Future Consideration**: When adding web API, use HTTPS
```cpp
// For future REST API implementation
// Use libraries like cpp-httplib with SSL/TLS
// #include <httplib.h>
// 
// httplib::SSLServer svr("cert.pem", "key.pem");
```

#### 7. Insufficient Logging 🟡
**Current Implementation**: Minimal logging

**Solution**: Implement security event logging
```cpp
class SecurityLogger {
public:
    static void logLoginAttempt(const std::string& username, bool success) {
        std::ofstream log("data/logs/security.log", std::ios::app);
        log << getCurrentTimestamp() << " | LOGIN_ATTEMPT | "
            << username << " | " << (success ? "SUCCESS" : "FAILED") << "\n";
    }
    
    static void logDataAccess(ull userID, const std::string& resource) {
        std::ofstream log("data/logs/audit.log", std::ios::app);
        log << getCurrentTimestamp() << " | DATA_ACCESS | "
            << userID << " | " << resource << "\n";
    }
};
```

### Low Priority Issues

#### 8. No Account Lockout 🟢
**Solution**: Lock account after repeated failed login attempts

#### 9. No Password Complexity Requirements 🟢
**Solution**: Enforce strong password policies

#### 10. No Email Verification 🟢
**Solution**: Add email verification for registration (requires email service)

## Recommended Security Enhancements

### Priority 1: Authentication & Authorization
```cpp
class SecureAuthManager {
private:
    HashMap<std::string, std::string> passwordHashes; // username -> hash
    HashMap<std::string, std::string> salts;         // username -> salt
    RateLimiter rateLimiter;
    
public:
    bool registerUser(const std::string& username, const std::string& password) {
        // Validate password strength
        if (!isStrongPassword(password)) {
            return false;
        }
        
        // Generate salt
        std::string salt = generateSalt();
        
        // Hash password with salt
        std::string hash = hashPassword(password, salt);
        
        // Store securely
        passwordHashes[username] = hash;
        salts[username] = salt;
        
        return true;
    }
    
    bool authenticateUser(const std::string& username, const std::string& password) {
        // Check rate limit
        if (!rateLimiter.checkLimit(username)) {
            SecurityLogger::logLoginAttempt(username, false);
            return false; // Too many attempts
        }
        
        // Verify credentials
        if (!passwordHashes.contains(username)) {
            SecurityLogger::logLoginAttempt(username, false);
            return false;
        }
        
        std::string hash = hashPassword(password, salts[username]);
        bool success = (hash == passwordHashes[username]);
        
        SecurityLogger::logLoginAttempt(username, success);
        return success;
    }
    
private:
    bool isStrongPassword(const std::string& password) {
        if (password.length() < 8) return false;
        
        bool hasUpper = false, hasLower = false;
        bool hasDigit = false, hasSpecial = false;
        
        for (char c : password) {
            if (isupper(c)) hasUpper = true;
            if (islower(c)) hasLower = true;
            if (isdigit(c)) hasDigit = true;
            if (!isalnum(c)) hasSpecial = true;
        }
        
        return hasUpper && hasLower && hasDigit && hasSpecial;
    }
    
    std::string generateSalt() {
        // Use cryptographically secure random
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        std::stringstream ss;
        for (int i = 0; i < 32; i++) {
            ss << std::hex << dis(gen);
        }
        return ss.str();
    }
};
```

### Priority 2: Data Protection
```cpp
class SecureDataManager {
public:
    // Encrypt sensitive data before writing
    void saveEncrypted(const std::string& filename, const std::string& data) {
        std::string encrypted = encrypt(data, getEncryptionKey());
        std::ofstream file(filename, std::ios::binary);
        file << encrypted;
        chmod(filename.c_str(), S_IRUSR | S_IWUSR);
    }
    
    // Decrypt when reading
    std::string loadEncrypted(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        std::string encrypted((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        return decrypt(encrypted, getEncryptionKey());
    }
    
private:
    std::string getEncryptionKey() {
        // In production, use environment variable or key management service
        // NEVER hardcode encryption keys in source code
        const char* key = std::getenv("APP_ENCRYPTION_KEY");
        return key ? key : "DEFAULT_KEY_CHANGE_ME";
    }
};
```

### Priority 3: Input Validation
```cpp
class InputValidator {
public:
    static bool validateUsername(const std::string& username) {
        // Length check
        if (username.length() < 3 || username.length() > 20) {
            return false;
        }
        
        // Character check (alphanumeric and underscore only)
        for (char c : username) {
            if (!isalnum(c) && c != '_') {
                return false;
            }
        }
        
        // Prevent SQL injection patterns (if SQL is added later)
        std::string lower = toLower(username);
        if (lower.find("select") != std::string::npos ||
            lower.find("drop") != std::string::npos ||
            lower.find("union") != std::string::npos) {
            return false;
        }
        
        return true;
    }
    
    static std::string sanitizePostContent(const std::string& content) {
        std::string clean;
        for (char c : content) {
            // Allow printable ASCII except < and > to prevent XSS
            if (c >= 32 && c <= 126 && c != '<' && c != '>') {
                clean += c;
            } else if (c == '\n' || c == '\r') {
                clean += c; // Allow newlines
            }
        }
        return clean;
    }
};
```

## Security Checklist for Production

- [ ] Implement password hashing (bcrypt/argon2)
- [ ] Add input sanitization for all user inputs
- [ ] Implement session management with secure tokens
- [ ] Add rate limiting for login attempts
- [ ] Set proper file permissions on data files
- [ ] Implement comprehensive logging (security events)
- [ ] Add email verification for new accounts
- [ ] Implement account lockout after failed attempts
- [ ] Use HTTPS for all network communication (future)
- [ ] Encrypt sensitive data at rest
- [ ] Add CSRF protection (for web interface)
- [ ] Implement proper error handling (don't leak info)
- [ ] Add database parameterized queries (if SQL added)
- [ ] Regular security audits and penetration testing
- [ ] Keep dependencies updated
- [ ] Implement backup and recovery procedures
- [ ] Add two-factor authentication (2FA)
- [ ] Implement data retention policies
- [ ] Add GDPR compliance features (data export/deletion)
- [ ] Security headers (for web API)

## External Libraries for Security

### Recommended Libraries

1. **OpenSSL** - Cryptography
   ```bash
   sudo apt-get install libssl-dev
   ```

2. **Bcrypt** - Password hashing
   ```bash
   # Use bcrypt library for C++
   git clone https://github.com/trusch/libbcrypt
   ```

3. **Botan** - Cryptography library
   ```bash
   sudo apt-get install libbotan-2-dev
   ```

4. **cpp-jwt** - JWT tokens for API
   ```bash
   # Header-only library
   git clone https://github.com/arun11299/cpp-jwt
   ```

## Compliance Considerations

### GDPR Compliance (if EU users)
- Right to access (data export)
- Right to erasure (account deletion)
- Right to rectification (data correction)
- Data portability
- Consent management

### General Best Practices
- Principle of least privilege
- Defense in depth
- Fail securely (errors don't expose info)
- Regular security updates
- Security by design

## Incident Response Plan

1. **Detection**: Monitor logs for suspicious activity
2. **Containment**: Disable affected accounts
3. **Investigation**: Analyze security logs
4. **Recovery**: Restore from backups if needed
5. **Prevention**: Update security measures

## Conclusion

This educational project demonstrates data structures effectively but **must not** be used in production without addressing these security concerns. For a production social media platform, consider using:

- Industry-standard frameworks (ASP.NET, Django, Spring Boot)
- Established databases (PostgreSQL with proper encryption)
- Professional security auditing
- Compliance with relevant regulations

Remember: **Security is not a feature, it's a requirement.**
